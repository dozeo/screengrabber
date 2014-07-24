#ifdef WIN32
#include <windows.h>

#include "window_info.h"

namespace dz {

/// Gets passed to EnumWindowProc
struct EnumerateWindowSettings {
	std::vector<WindowInfo> * destination;
	int64_t pid;
};

static BOOL CALLBACK EnumWindowsProc (HWND win, LPARAM user){
	EnumerateWindowSettings * settings = (EnumerateWindowSettings*) (user);

	DWORD process;
	GetWindowThreadProcessId (win, &process);

	if (settings->pid && settings->pid != process) {
		return TRUE; // skipping
	}

	if (process == GetCurrentProcessId())
		return TRUE;

	WINDOWINFO info;
	GetWindowInfo(win, &info);

	char title [512] = "";
	GetWindowText (win, title, (sizeof title) - 1);

	bool isVisible = (info.dwStyle & WS_VISIBLE) != 0;
	bool isMinimized = (info.dwStyle & WS_MINIMIZE) == 0;
	bool isNotDisabled = (info.dwStyle & WS_DISABLED) == 0;
	bool isTopLevel = (GetWindowLong(win, GWL_HWNDPARENT) == NULL) || ((HWND)GetWindowLong(win, GWL_HWNDPARENT) == GetDesktopWindow());
	bool isNotTool = (info.dwExStyle & WS_EX_TOOLWINDOW) == 0;

	if (isVisible && isTopLevel && isNotDisabled && isNotTool)
	{
		WindowInfo dst;
		dst.title = title;
		dst.id    = (int64_t) win; // At least in Win7 Window Handles seem unique and could also be interpreted as IDs
		dst.pid   = process;
		dst.area  = dz::Rect(info.rcWindow.left, info.rcWindow.top, info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top);
		dst.is_minimized = isMinimized;
		if (dst.area.width == 0 || dst.area.height == 0)
			return TRUE;

		settings->destination->push_back (dst);
	}
	return TRUE; // continue enumeration
}

void WindowInfo::populate(std::vector<WindowInfo> * destination, int64_t pid) 
{
	destination->clear();
	EnumerateWindowSettings settings;
	settings.destination = destination;
	settings.pid = pid;
	EnumWindows (&EnumWindowsProc, (LPARAM) &settings);
}

/*static*/ WindowInfo WindowInfo::about (int64_t wid){
	HWND win = (HWND) (wid);
	WINDOWINFO info;
	GetWindowInfo(win, &info);

	bool isVisible = (info.dwStyle & WS_VISIBLE) != 0;

	if (isVisible) {
		char title [512] = "";
		GetWindowText (win, title, (sizeof title) - 1);

		DWORD process;
		GetWindowThreadProcessId (win, &process);


		WindowInfo dst;
		dst.title = title;
		dst.id    = (int64_t) win; // At least in Win7 Window Handles seem unique and could also be interpreted as IDs
		dst.pid   = process;
		dst.area  = dz::Rect(info.rcWindow.left, info.rcWindow.top, info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top);
		return dst;
	} else {
		// do not count invisible windows
		return WindowInfo ();
	}
}

}

#endif