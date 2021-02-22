#include "WinWindow.h"

#include <Windows.h>
#include "GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui_impl_win32.h"

WinWindow::WinWindow(HINSTANCE instance, std::wstring className, int width, int height)
	: m_errorString()
	, m_errorCode(ErrorCode::NONE)
	, m_className(className)
	, m_title(className)
	, m_width(width)
	, m_height(height)
	, m_destroyed(false)
	, m_fullscreen(false)
	, m_hndl(nullptr)
	, m_deviceContext(nullptr)
	, m_renderContext(nullptr)
	, m_instance(instance)
{
}

WinWindow::~WinWindow()
{
}

void WinWindow::Init()
{
	RegisterWindowInternal();
	CreateWindowInternal();
	CreateContext();

	if (m_errorCode == ErrorCode::NONE)
	{
		wglMakeCurrent(m_deviceContext, m_renderContext);
		ShowWindow(m_hndl, SW_SHOW);
		CenterWindow();
		UpdateWindow(m_hndl);
	}
}

void WinWindow::Destroy()
{
	wglMakeCurrent(m_deviceContext, NULL);
	wglDeleteContext(m_renderContext);
	ReleaseDC(m_hndl, m_deviceContext);
	DestroyWindow(m_hndl);
	m_destroyed = true;
}

bool WinWindow::HasError() const
{
	return m_errorCode != ErrorCode::NONE;
}

IWindow::ErrorCode WinWindow::GetError() const
{
	return m_errorCode;
}

const std::string& WinWindow::GetErrorString() const
{
	return m_errorString;
}

void WinWindow::SwapBuffer()
{
	glFlush();
	::SwapBuffers(m_deviceContext);
}

bool WinWindow::IsValid() const
{
	return !m_destroyed;
}

void WinWindow::SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback)
{
	m_onWindowSizeChanged = callback;
}

void WinWindow::SetWinProcHandle(const WinProcHandle& handle)
{
	m_onWndProc = handle;
}

HWND WinWindow::GetHWND() const
{
	return m_hndl;
}

void WinWindow::RegisterWindowInternal()
{
	WNDCLASSEXW wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)&WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_instance;
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_className.c_str();
	wcex.hIconSm = NULL;

	if (!RegisterClassExW(&wcex))
	{
		m_errorCode = ErrorCode::ERR_SYSCALL;
		m_errorString = "RegisterClassExW failed: Can not register window class.";
	}
}

void WinWindow::CreateWindowInternal()
{
	if (m_errorCode == ErrorCode::NONE)
	{
		DWORD  style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		RECT rect = { 0, 0, m_width, m_height };

		if (AdjustWindowRect(&rect, style, false))
		{
			LPCWSTR className = m_className.c_str();
			LPCWSTR tittle = m_title.c_str();
			m_hndl = CreateWindowW(className, tittle, style, 0, 0, m_width, m_height, nullptr, nullptr, m_instance, nullptr);
			if (!m_hndl)
			{
				m_errorCode = ErrorCode::ERR_SYSCALL;
				m_errorString = "CreateWindowW failed: Can not create window.";
			}
			else
			{
				LONG_PTR ptr = reinterpret_cast<LONG_PTR>(this);
				SetWindowLongPtrW(m_hndl, GWLP_USERDATA, ptr);
			}
		}
		else
		{
			m_errorCode = ErrorCode::ERR_SYSCALL;
			m_errorString = "AdjustWindowRect failed: Can not create window.";
		}
	}
}

void WinWindow::CreateContext()
{
	if (m_errorCode != ErrorCode::NONE)
	{
		return;
	}

	m_deviceContext = GetDC(m_hndl);
	if (!m_deviceContext)
	{
		m_errorCode = ErrorCode::ERR_GL;
		m_errorString = "GetDC failed: Can not create device context.";
		return;
	}

	int pixelFormat;
	PIXELFORMATDESCRIPTOR pixelFormatDesc = {};

	pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormatDesc.nVersion = 1;
	pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDesc.cColorBits = 32;
	pixelFormatDesc.cAlphaBits = 8;
	pixelFormatDesc.cDepthBits = 24;

	pixelFormat = ChoosePixelFormat(m_deviceContext, &pixelFormatDesc);
	if (pixelFormat == 0)
	{
		m_errorCode = ErrorCode::ERR_GL;
		m_errorString = "ChoosePixelFormat failed: Can not create render context.";
		return;
	}

	if (SetPixelFormat(m_deviceContext, pixelFormat, &pixelFormatDesc))
	{
		m_renderContext = wglCreateContext(m_deviceContext);
		if (!m_renderContext)
		{
			m_errorCode = ErrorCode::ERR_GL;
			m_errorString = "wglCreateContext failed: Can not create render context.";
		}
	}
	else
	{
		m_errorCode = ErrorCode::ERR_GL;
		m_errorString = "SetPixelFormat failed: Can not create render context.";
	}
}

void WinWindow::CenterWindow()
{
	MONITORINFO mi = {};
	mi.cbSize = sizeof(mi);

	GetMonitorInfo(MonitorFromWindow(m_hndl, MONITOR_DEFAULTTONEAREST), &mi);
	int x = (mi.rcMonitor.right - mi.rcMonitor.left - m_width) / 2;
	int y = (mi.rcMonitor.bottom - mi.rcMonitor.top - m_height) / 2;

	SetWindowPos(m_hndl, 0, x, y, m_width, m_height, SWP_NOZORDER  | SWP_SHOWWINDOW);
}

void WinWindow::SetFullscreen(bool fullscreen)
{
	DWORD style = GetWindowLong(m_hndl, GWL_STYLE);
	m_fullscreen = fullscreen;
	if (fullscreen)
	{
		MONITORINFO mi = {};
		mi.cbSize = sizeof(mi);

		GetMonitorInfo(MonitorFromWindow(m_hndl, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(m_hndl, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hndl, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
	else
	{
		MONITORINFO mi = { sizeof(mi) };
		UINT flags = SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW;
		GetMonitorInfo(MonitorFromWindow(m_hndl, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(m_hndl, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);

		CenterWindow();
	}
}

LRESULT WinWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR ptr = GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	WinWindow* winWindow = reinterpret_cast<WinWindow*>(ptr);

	LRESULT result = 0;

	if (winWindow != nullptr && winWindow->m_onWndProc != nullptr)
	{
		winWindow->m_onWndProc(hWnd, message, wParam, lParam);
	}

	result = DefWindowProc(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
	{
		math::vec2f newSize;
		newSize.width = LOWORD(lParam);
		newSize.height = HIWORD(lParam);
		if (winWindow)
		{
			winWindow->m_onWindowSizeChanged(newSize);
		}
		glViewport(0, 0, static_cast<GLint>(newSize.width), static_cast<GLint>(newSize.height));
	}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else if (wParam == VK_F11)
		{
			if (winWindow)
			{
				winWindow->SetFullscreen(!winWindow->m_fullscreen);
			}
		}
		break;
	case WM_CLOSE:
		if (winWindow)
		{
			winWindow->Destroy();
		}
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return result;
}
