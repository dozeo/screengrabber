#ifdef WIN32

#include "Window.h"
#include <assert.h>

namespace dz
{
	Dimension2 Window::desktopSize(int screen)
	{
		int desktopWidth  = GetSystemMetrics(SM_CXSCREEN);
		int desktopHeight = GetSystemMetrics(SM_CYSCREEN);
		return Dimension2(desktopWidth, desktopHeight);
	}

	Dimension2 Window::windowSize(HWND hWnd)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return Dimension2(rect.right - rect.left, rect.bottom - rect.top);
	}

	Dimension2 Window::windowClientSize(HWND hWnd)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return Dimension2(rect.right - rect.left, rect.bottom - rect.top);
	}

	Window::Window() : _title("hidden"), _hWnd(0)
	{
		Rect rect(0, 0, 10, 10);
		create(rect);
	}

	Window::~Window()
	{
		shutdown();
	}

	HWND Window::getHWnd() const
	{
		return _hWnd;
	}

	LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void Window::create(const Rect& rect)
	{
		const char* title = _title.c_str();
		_windowClass.cbSize        = sizeof(_windowClass);
		_windowClass.style         = CS_VREDRAW | CS_HREDRAW;
		_windowClass.lpfnWndProc   = WndProc;
		_windowClass.cbClsExtra    = 0;
		_windowClass.cbWndExtra    = 0;
		_windowClass.hInstance     = 0;
		_windowClass.hIcon         = NULL;
		_windowClass.hCursor       = NULL;
		_windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		_windowClass.lpszMenuName  = title;
		_windowClass.lpszClassName = title;
		_windowClass.hIconSm       = NULL;
		RegisterClassEx(&_windowClass);

		DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW;

		HINSTANCE  hInstance = GetModuleHandle(NULL);
		_hWnd = CreateWindowEx(
			0,
			title,
			title,
			dwWindowStyle,
			rect.x,
			rect.y,
			rect.w,
			rect.h,
			NULL,
			NULL,
			hInstance,
			this);

		if (_hWnd == NULL)
		{
			throw std::exception("Could not create window!");
		}
	}

	void Window::shutdown()
	{
		if (_hWnd != 0)
		{
			DestroyWindow(_hWnd);
			UnregisterClass(_title.c_str(), _windowClass.hInstance);
			_hWnd = 0;
		}
	}
}

#endif
