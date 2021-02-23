#include "MandelbrotCPURender.h"

#include <stdexcept>
#include <cmath>
#include <gl/glew.h>

#include "Palette.h"

const size_t MandelbrotCPURender::s_sizeofRGBA = 4;
const size_t MandelbrotCPURender::s_offesetR = 0;
const size_t MandelbrotCPURender::s_offesetG = 1;
const size_t MandelbrotCPURender::s_offesetB = 2;
const size_t MandelbrotCPURender::s_offesetA = 3;

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
			if (m_isBusy.load(std::memory_order_relaxed) || m_mainThread.joinable())
			{
				StopMainWorker();
				CleanupMainWorker();
			}

			StartMainWorker();
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
		glDrawPixels(m_currentResolution.width, m_currentResolution.height, GL_RGBA, GL_UNSIGNED_BYTE, m_bufferData.get());
		glFlush();
	}
}

void MandelbrotCPURender::StartMainWorker()
{
	m_cancelRequested.store(false, std::memory_order_relaxed);
	m_isBusy.store(true, std::memory_order_relaxed);

	if (std::shared_ptr<RenderConfig> config = GetData())
	{
		m_currentResolution.width = static_cast<int>(config->m_windowSize.width);
		m_currentResolution.height = static_cast<int>(config->m_windowSize.height);
		m_sizeData = s_sizeofRGBA * m_currentResolution.width * m_currentResolution.height;
		if (m_maxSizeData < m_sizeData)
		{
			MakeBufferData(m_sizeData);
		}
		else
		{
			std::memset(m_bufferData.get(), 0, m_sizeData);
		}
		m_mainThread = std::thread(&MandelbrotCPURender::MainWorker, this, *config);
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
		poolThread.emplace_back(std::thread(&MandelbrotCPURender::Worker, this, std::ref(copyConfig), i, maxThread));
	}
	for (std::thread& th : poolThread)
	{
		th.join();
	}
	poolThread.clear();
	m_isBusy.store(false, std::memory_order_relaxed);
}

void MandelbrotCPURender::Worker(const RenderConfig& refConfig, const int workerID, const int threadCount)
{
	const double scale = 1.0 / refConfig.m_zoom;
	const size_t width = static_cast<unsigned long long>(refConfig.m_windowSize.width);
	const size_t height = static_cast<unsigned long long>(refConfig.m_windowSize.height);
	const math::vec2d resolution = math::toVec2d(refConfig.m_windowSize);
	const math::vec2d position = math::toVec2d(refConfig.m_position);
	const float threshold = refConfig.m_threshold;
	const float maxIterations = static_cast<float>(refConfig.m_maxIterations);

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

			double c2 = dot(c, c);
			// skip computation inside M1 - http://iquilezles.org/www/articles/mset_1bulb/mset1bulb.htm
			if ((256.0 * c2 * c2 - 96.0 * c2 + 32.0 * c.x - 3.0 < 0.0)
				// skip computation inside M2 - http://iquilezles.org/www/articles/mset_2bulb/mset2bulb.htm
				&& (16.0 * (c2 + 2.0 * c.x + 1.0) - 1.0 < 0.0))
			{
				iterations = 0;
			}
			else
			{
				while (iterations <= maxIterations)
				{
					// Z -> Z² + c
					z = math::vec2d(std::pow(z.x, 2) - std::pow(z.y, 2), 2 * z.x * z.y) + c;

					if (math::dot(z, z) > threshold)
						break;

					++iterations;
				}
			}

			if (iterations < maxIterations) 
			{
				iterations += 1 - std::log(math::dot(z, z)) / std::log(threshold);
			}
			else
			{
				iterations = 0;
			}

			iterations += OFFSET_COLOR;
			double it = 0.0;
			const double fraction = modf(iterations, &it);

			const int* color1 = PALETTE[static_cast<size_t>(it) % PALETTE_SIZE];
			const int* color2 = PALETTE[static_cast<size_t>(it) % PALETTE_SIZE];
			
			const double r = std::lerp(color1[0], color2[0], fraction);
			const double g = std::lerp(color1[1], color2[1], fraction);
			const double b = std::lerp(color1[2], color2[2], fraction);

			if (x > width)
			{
				throw std::out_of_range("x position is invalid");
			}

			if (y > height)
			{
				throw std::out_of_range("y position is invalid");
			}

			const size_t pos = (x + y * width) * s_sizeofRGBA;

			if ((pos + s_offesetA) < m_sizeData && (pos + s_offesetA) < m_maxSizeData)
			{
				m_bufferData[pos + s_offesetR] = static_cast<unsigned char>(std::round(r));
				m_bufferData[pos + s_offesetG] = static_cast<unsigned char>(std::round(g));
				m_bufferData[pos + s_offesetB] = static_cast<unsigned char>(std::round(b));
				m_bufferData[pos + s_offesetA] = 255;
			}
			else
			{
				throw std::out_of_range("position is invalid");
			}

			
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
	std::memset(m_bufferData.get(), 0, m_sizeData);
}
