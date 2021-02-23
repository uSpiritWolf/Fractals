#include "MandelbrotCPURender.h"

#include <cmath>
#include <gl/glew.h>

MandelbrotCPURender::MandelbrotCPURender()
	: m_isBusy(false)
	, m_cancelRequested(false)
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
	if (m_data)
	{
		glDrawPixels(m_currentResolution.width, m_currentResolution.height, GL_RGB, GL_UNSIGNED_BYTE, m_data.get());
	}
}

void MandelbrotCPURender::StartMainWorker()
{
	m_cancelRequested.store(false, std::memory_order_relaxed);
	m_isBusy.store(true, std::memory_order_relaxed);

	if (std::shared_ptr<RenderConfig> config = GetData())
	{
		int width = static_cast<int>(config->m_windowSize.width);
		int height = static_cast<int>(config->m_windowSize.height);

		if (width != m_currentResolution.width || height != m_currentResolution.height)
		{
			m_currentResolution.width = width;
			m_currentResolution.height = height;
			m_sizeData = static_cast<size_t>(3) * m_currentResolution.width * m_currentResolution.height;
			m_data.reset(new unsigned char[m_sizeData]);
		}

		std::memset(m_data.get(), 0, m_sizeData);
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
		poolThread.emplace_back(std::thread(&MandelbrotCPURender::Worker, this, copyConfig, i, maxThread));
	}
	for (std::thread& th : poolThread)
	{
		th.join();
	}
	poolThread.clear();
}

void MandelbrotCPURender::Worker(const RenderConfig copyConfig, const int workerID, const int threadCount)
{
	float scale = 1.0f / copyConfig.m_zoom;
	unsigned long long width = static_cast<unsigned long long>(copyConfig.m_windowSize.width);
	unsigned long long height = static_cast<unsigned long long>(copyConfig.m_windowSize.height);
	math::vec2d resolution = math::toVec2d(copyConfig.m_windowSize);
	math::vec2d position = math::toVec2d(copyConfig.m_position);
	float threshold = copyConfig.m_threshold;
	float maxIterations = static_cast<float>(copyConfig.m_maxIterations);

	bool canceled = m_cancelRequested.load(std::memory_order_relaxed);
	for (int y = workerID; y < height && !canceled; y += threadCount)
	{
		for (int x = 0; x < width && !canceled; ++x)
		{
			if (canceled = m_cancelRequested.load(std::memory_order_relaxed))
				break;

			math::vec2d coord(x, y);

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
				while (!canceled && iterations <= maxIterations)
				{
					// Z -> Z² + c
					z = math::vec2d(std::pow(z.x, 2) - std::pow(z.y, 2), 2 * z.x * z.y) + c;

					if (math::dot(z, z) > threshold)
						break;

					++iterations;
				}
			}

			unsigned long long pos = (x + y * width) * 3;
			m_data[pos + 0] = 0; // R
			m_data[pos + 1] = static_cast<unsigned char>((iterations / maxIterations) * 255); // G
			m_data[pos + 2] = 0; // B
		}
	}

	if (canceled)
	{
		std::memset(m_data.get(), 0, m_sizeData);
	}
}
