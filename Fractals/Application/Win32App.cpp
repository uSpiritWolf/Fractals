#if defined(WIN32)

#include "Win32App.h"
#include <Windows.h>

#include "UI/WinWindow.h"
#include "Graphics/FractalsRender.h"
#include "Logger/Logger.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Win32App::Win32App()
	: m_mainWindow(new WinWindow(GetModuleHandle(NULL), L"Mandelbrot", 1280, 720))
	, m_frameTime(33)
	, m_onUpdate(nullptr)
	, m_onRender(nullptr)
	, m_onWindowSizeChanged(nullptr)
	, m_prevTime(0)
{
}

Win32App::~Win32App()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	ImGui_ImplWin32_Shutdown();
}

int Win32App::Run()
{
	Logger::Log(LogLevel::INFO, "Lock frame time: " + std::to_string(m_frameTime) + "ms" + "  (" + std::to_string(1.0f/(m_frameTime/1000.0f)) + " FPS)");

	MSG msg = {};
	ULONGLONG  dwNextDeadLine = GetTickCount64() + m_frameTime;
	DWORD  dwSleep = m_frameTime;
	bool bUpdate = false;
	bool exit = false;

	while (!exit)
	{
		DWORD dwResult = MsgWaitForMultipleObjectsEx(0, NULL, dwSleep, QS_ALLEVENTS, 0);
		if (dwResult != WAIT_TIMEOUT)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					exit = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			bUpdate = GetTickCount64() >= dwNextDeadLine;
		}
		else
		{
			bUpdate = true;
		}

		if (bUpdate)
		{
			UpdateAndRender();
		}

		dwSleep = static_cast<DWORD>(dwNextDeadLine - GetTickCount64());
		if (dwSleep > m_frameTime)
		{
			dwSleep = m_frameTime;
			dwNextDeadLine = GetTickCount64() + m_frameTime;
		}
	}
	return 0;
}

void Win32App::Init()
{
	m_mainWindow->SetWindowSizeChanged([this](const math::vec2f& newSize)
	{
		if (m_onWindowSizeChanged)
		{
			m_onWindowSizeChanged(newSize);
		}
	});

	m_mainWindow->Init();

	if (m_mainWindow->HasError())
	{
		Logger::Log(LogLevel::ERR, m_mainWindow->GetErrorString());
	}
	else
	{
		InitImGui();
	}
}

void Win32App::SetUpdateCallback(const std::function<void(float dt)>& callback)
{
	m_onUpdate = callback;
}

void Win32App::SetRenderCallback(const std::function<void()>& callback)
{
	m_onRender = callback;
}

void Win32App::SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback)
{
	m_onWindowSizeChanged = callback;
}

void Win32App::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_mainWindow->GetHWND());
	ImGui_ImplOpenGL3_Init("#version 450");
	ImGui::StyleColorsDark();

	m_mainWindow->SetWinProcHandle([](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	});
}

void Win32App::UpdateAndRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	if (m_onUpdate)
	{
		ULONGLONG currentTick = GetTickCount64();

		if (m_prevTime == 0)
			m_prevTime = currentTick;

		float dt = (currentTick - m_prevTime) / 1000.0f; // ms to sec
		m_prevTime = currentTick;
		m_onUpdate(dt);
	}

	if (m_onRender)
	{
		m_onRender();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (m_mainWindow && m_mainWindow->IsValid())
	{
		m_mainWindow->SwapBuffer();
	}
}

#endif