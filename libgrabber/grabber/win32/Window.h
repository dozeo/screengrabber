#pragma once

#ifdef WIN32

#include "../Dimension.h"
#include "../Position.h"
#include "../Rect.h"

#include <iostream>
#include <windows.h>

namespace dz
{

class Window
{
public:
	static Dimension2 desktopSize(int screen);
	static Dimension2 windowSize(HWND hWnd);
	static Dimension2 windowClientSize(HWND hWnd);

	Window();
	virtual ~Window();

	HWND getHWnd() const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void create(const Rect& rect);
	void shutdown();

	WNDCLASSEX    _windowClass;
	HWND          _hWnd;
	std::string   _title;
};

}
#endif
