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
	
	bool IsBusy() const;

private:
	void StartMainWorker();
	void StopMainWorker();
	void CleanupMainWorker();

	void MainWorker(const RenderConfig copyConfig);
	void WorkerColorDraw(const RenderConfig& refConfig, const int workerID, const int threadCount);
	void WorkerGrayDraw(const RenderConfig& refConfig, const int workerID, const int threadCount);

	void MakeBufferData(size_t size);

	std::thread m_mainThread;

	std::atomic<bool> m_cancelRequested;
	std::atomic<bool> m_isBusy;

	RenderConfig m_prevConfig;

	math::vec2i m_currentResolution;

	size_t m_sizeData;
	size_t m_maxSizeData;
	std::unique_ptr<unsigned char[]> m_bufferData;

	static const size_t s_sizeofRGB;
	static const size_t s_offesetR;
	static const size_t s_offesetG;
	static const size_t s_offesetB;
};