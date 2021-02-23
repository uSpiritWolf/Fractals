#pragma once

#include <thread>
#include <vector>
#include <atomic>

#include "Data/RenderConfig.h"
#include "Data/DataBinder.h"
#include "Math/vec.h"

struct RenderConfig;

class MandelbrotCPURender : public DataBinder<RenderConfig>
{
public:
	MandelbrotCPURender();
	~MandelbrotCPURender();

	void OnUpdate();
	void OnRender();

private:
	void StartMainWorker();
	void StopMainWorker();
	void CleanupMainWorker();

	void MainWorker(const RenderConfig copyConfig);
	void Worker(const RenderConfig copyConfig, const int workerID, const int threadCount);

	std::thread m_mainThread;

	std::atomic<bool> m_cancelRequested;
	std::atomic<bool> m_isBusy;

	RenderConfig m_prevConfig;

	math::vec2i m_currentResolution;
	size_t m_sizeData;
	std::unique_ptr<unsigned char[]> m_data;
};