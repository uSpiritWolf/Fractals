#pragma once
#if defined(WIN32)

#include "IApp.h"
#include <functional>
#include <memory>

class WinWindow;
class ILogger;

class Win32App : public IApp
{
public:
	Win32App();
	virtual ~Win32App();

	int Run() override;

	void Init() override;

	void SetUpdateCallback(const std::function<void(float dt)>& callback) override;
	void SetRenderCallback(const std::function<void()>& callback) override;
	void SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback) override;

private:
	void InitImGui();

	void UpdateAndRender();

	std::unique_ptr<WinWindow> m_mainWindow;

	unsigned long m_frameTime;

	unsigned long long m_prevTime;

	std::function<void(float dt)> m_onUpdate;
	std::function<void()> m_onRender;
	std::function<void(math::vec2f newSize)> m_onWindowSizeChanged;
};

#endif