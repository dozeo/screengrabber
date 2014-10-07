#pragma once

#ifdef WIN32

#include <libcommon/dzrect.h>
#include <vector>
#include <Windows.h>

namespace dz
{
	struct Display
	{
		Display(HMONITOR _hMonitor, const Rect& _rect) : hMonitor(_hMonitor), rect(_rect) {}
		HMONITOR hMonitor;
		Rect rect;
	};

	class ScreenEnumerator
	{
		public:
			std::vector<Display> displays() const;
			int screen(const Rect& screenRect) const;

			bool enumerate();

		private:
			static BOOL CALLBACK monitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM data);

			std::vector<Display> _displays;
	};
}

#endif