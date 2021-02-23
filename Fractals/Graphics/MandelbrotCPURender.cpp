#include "MandelbrotCPURender.h"

#include <stdexcept>
#include <cmath>
#include <gl/glew.h>

#include "Palette.h"

const size_t MandelbrotCPURender::s_sizeofRGB = 3;
const size_t MandelbrotCPURender::s_offesetR = 0;
const size_t MandelbrotCPURender::s_offesetG = 1;
const size_t MandelbrotCPURender::s_offesetB = 2;

MandelbrotCPURender::MandelbrotCPURender()
	: m_isBusy(false)
	, m_cancelRequested(false)
	, m_sizeData(0)
	, m_maxSizeData(0)
{

}

MandelbrotCPURender::~MandelbrotCPURender()
{
	if (m_isBusy.load(std::memory_order_relaxed) || m_mainThread.joinable())
	{
		StopMainWorker();
		CleanupMainWorker();
	}
}

void MandelbrotCPURender::OnUpdate()
{
	if (std::shared_ptr<RenderConfig> config = GetData())
	{
		if (m_prevConfig != *config)
		{
			m_prevConfig = *config;
			if (IsBusy())
			{
				StopMainWorker();
				CleanupMainWorker();
			}

			if (config->m_useCPU)
			{
				StartMainWorker();
			}
		}
	}

	if (!m_isBusy.load(std::memory_order_relaxed) && m_mainThread.joinable())
	{
		CleanupMainWorker();
	}
}

void MandelbrotCPURender::OnRender()
{
	if (m_bufferData)
	{
		glRasterPos2f(-1.f, -1.f);
		glPixelStoref(GL_PACK_ALIGNMENT, 1);
		glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
		glDrawPixels(m_currentResolution.width, m_currentResolution.height, GL_RGB, GL_UNSIGNED_BYTE, m_bufferData.get());
		glFlush();
	}
}

bool MandelbrotCPURender::IsBusy() const
{
	return m_mainThread.joinable() || m_isBusy.load(std::memory_order_relaxed);
}

void MandelbrotCPURender::StartMainWorker()
{
	if (std::shared_ptr<RenderConfig> config = GetData())
	{
		int width = static_cast<int>(config->m_windowSize.width);
		int height = static_cast<int>(config->m_windowSize.height);
		m_sizeData = s_sizeofRGB * width * height;
		if (m_maxSizeData < m_sizeData)
		{
			MakeBufferData(m_sizeData);
		}
		else
		{
			std::memset(m_bufferData.get(), 0, m_sizeData);
		}
		m_currentResolution.width = width;
		m_currentResolution.height = height;

		m_cancelRequested.store(false, std::memory_order_relaxed);
		m_mainThread = std::thread(&MandelbrotCPURender::MainWorker, this, *config);
		m_isBusy.store(true, std::memory_order_relaxed);
	}
}

void MandelbrotCPURender::StopMainWorker()
{
	m_cancelRequested.store(true, std::memory_order_relaxed);
}

void MandelbrotCPURender::CleanupMainWorker()
{
	if (m_mainThread.joinable())
	{
		m_mainThread.join();
	}
	m_cancelRequested.store(false, std::memory_order_relaxed);
	m_isBusy.store(false, std::memory_order_relaxed);
}

void MandelbrotCPURender::MainWorker(const RenderConfig copyConfig)
{
	const int maxThread = std::thread::hardware_concurrency() - 1;
	std::vector<std::thread> poolThread;
	for (int i = 0; i < maxThread; ++i)
	{
		poolThread.emplace_back(std::thread(&MandelbrotCPURender::WorkerColorDraw, this, std::ref(copyConfig), i, maxThread));
	}
	for (std::thread& th : poolThread)
	{
		th.join();
	}
	poolThread.clear();
	m_isBusy.store(false, std::memory_order_relaxed);
}

void MandelbrotCPURender::WorkerColorDraw(const RenderConfig& refConfig, const int workerID, const int threadCount)
{
	const double scale = 1.0 / refConfig.m_zoom;
	const size_t width = static_cast<unsigned long long>(refConfig.m_windowSize.width);
	const size_t height = static_cast<unsigned long long>(refConfig.m_windowSize.height);
	const math::vec2d resolution = math::toVec2d(refConfig.m_windowSize);
	const math::vec2d position = math::toVec2d(refConfig.m_position);
	const float threshold = refConfig.m_threshold;
	const float logthreshold = std::log(threshold);
	const int maxIterations = refConfig.m_maxIterations;

	bool canceled = false;

	for (size_t y = workerID; y < height; y += threadCount)
	{
		if (canceled = m_cancelRequested.load(std::memory_order_relaxed))
			break;

		for (size_t x = 0; x < width; ++x)
		{
			math::vec2d coord(static_cast<double>(x), static_cast<double>(y));

			math::vec2d c = scale * (2. * coord - resolution) / resolution.y - position;
			math::vec2d z;

			double iterations = 0;
			double lastDotProduct = 0;

			const double c2 = dot(c, c);
			// skip computation inside M1 - http://iquilezles.org/www/articles/mset_1bulb/mset1bulb.htm
			if ((256.0 * c2 * c2 - 96.0 * c2 + 32.0 * c.x - 3.0 < 0.0)
				// skip computation inside M2 - http://iquilezles.org/www/articles/mset_2bulb/mset2bulb.htm
				|| (16.0 * (c2 + 2.0 * c.x + 1.0) - 1.0 < 0.0))
			{
				iterations = 0.;
			}
			else
			{
				while (iterations <= maxIterations)
				{
					// Z -> Z² + c
					z = math::vec2d(z.x * z.x - z.y * z.y + c.x, 2 * z.x * z.y + c.y);
					//

					lastDotProduct = math::dot(z, z);
					if (lastDotProduct > threshold)
						break;

					++iterations;
				}
			}

			if (iterations != 0 && iterations < maxIterations)
			{
				iterations += 1 - std::log(lastDotProduct) / logthreshold;
			}
			else
			{
				iterations = 0;
			}

			iterations += OFFSET_COLOR;
			double it = 0.0;
			const double fraction = modf(iterations, &it);

			const int* color1 = PALETTE[static_cast<size_t>(it) % PALETTE_SIZE];
			const int* color2 = PALETTE[static_cast<size_t>(it + 1) % PALETTE_SIZE];
			
			const double r = std::lerp(color1[0], color2[0], fraction);
			const double g = std::lerp(color1[1], color2[1], fraction);
			const double b = std::lerp(color1[2], color2[2], fraction);

			const size_t pos = (x + y * width) * s_sizeofRGB;

			m_bufferData[pos + s_offesetR] = static_cast<unsigned char>(r);
			m_bufferData[pos + s_offesetG] = static_cast<unsigned char>(g);
			m_bufferData[pos + s_offesetB] = static_cast<unsigned char>(b);
		}
	}

	if (canceled)
	{
		std::memset(m_bufferData.get(), 0, m_sizeData);
	}
}

void MandelbrotCPURender::MakeBufferData(size_t size)
{
	m_maxSizeData = size;
	m_bufferData.reset(new unsigned char[size]);
	std::memset(m_bufferData.get(), 0, m_maxSizeData);
}
