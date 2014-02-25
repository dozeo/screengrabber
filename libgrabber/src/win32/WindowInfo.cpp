#ifdef WIN32
#include <windows.h>

#include "../WindowInfo.h"
#include "../ProcessInfo.h"

namespace dz
{
	/// Gets passed to EnumWindowProc
	struct EnumerateWindowSettings {
		std::vector<WindowInfo> * destination;
		int64_t pid;
	};

	static BOOL CALLBACK EnumWindowsProc (HWND win, LPARAM user)
	{
		EnumerateWindowSettings* settings = (EnumerateWindowSettings*) (user);

		DWORD process;
		GetWindowThreadProcessId (win, &process);

		if (settings->pid && settings->pid != process)
			return TRUE; // skipping

		ProcessInfo procInfo = ProcessInfo::about(process);

		WINDOWINFO info;
		GetWindowInfo(win, &info);

		char title[512] = "untitled";
		GetWindowText (win, title, (sizeof title) - 1);

		bool isVisible = (info.dwStyle & WS_VISIBLE) != 0;
		if (!isVisible)
			return TRUE;

		// figure out if the process owning the window is Windows\explorer.exe and ignore it if it is
		std::string winExplorer = "Windows\\explorer.exe";
		std::string procExec = procInfo.exec.substr( procInfo.exec.length() - winExplorer.length() );
		if (procExec.compare(winExplorer) == 0)
			return TRUE;

		int32_t width = info.rcWindow.right - info.rcWindow.left;
		int32_t height = info.rcWindow.bottom - info.rcWindow.top;
		//if (width <= 0 || height <= 0)
		//	return TRUE;

		std::string windowTitle = title;
		if (windowTitle.length() == 0)
		{
			std::ostringstream str;
			str << "Unnamed Window " << (int64_t)win;
			windowTitle = str.str();
		}

		WindowInfo dst;
		dst.title = windowTitle;
		dst.id = (int64_t)win; // At least in Win7 Window Handles seem unique and could also be interpreted as IDs
		dst.pid = process;
		dst.area  = dz::Rect(info.rcWindow.left, info.rcWindow.top, width, height);
		settings->destination->push_back (dst);

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