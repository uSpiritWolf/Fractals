#pragma once

#include "IWindow.h"

#include <Windows.h>

class WinWindow : public IWindow
{
public:
	using WinProcHandle = std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;


	WinWindow(HINSTANCE instance, std::wstring className, int width, int height);

	virtual ~WinWindow();

	void Init() override;

	void Destroy() override;

	bool HasError() const override;

	ErrorCode GetError() const override;

	const std::string& GetErrorString() const override;

	void SwapBuffer() override;

	bool IsValid() const override;

	void SetWindowSizeChanged(const std::function<void(const math::vec2f& newSize)>& callback) override;

	void SetWinProcHandle(const WinProcHandle& handle);

	HWND GetHWND() const;

private:
	void RegisterWindowInternal();
	void CreateWindowInternal();
	void CreateContext();

	void CenterWindow();
	void SetFullscreen(bool fullscreen);

	HINSTANCE		m_instance;
	HWND			m_hndl;
	HDC				m_deviceContext;
	HGLRC			m_renderContext;
	std::wstring	m_className;
	std::wstring	m_title;

	int				m_width;
	int				m_height;
	bool			m_fullscreen;

	std::string		m_errorString;
	ErrorCode		m_errorCode;

	bool			m_destroyed;

	std::function<void(math::vec2f newSize)> m_onWindowSizeChanged;

	WinProcHandle m_onWndProc;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};