#include "ScreenEnumerator.h"

#ifdef WIN32

using dz::Display;
using dz::Rect;
using dz::ScreenEnumerator;

bool ScreenEnumerator::enumerate()
{
	_displays.clear();
	EnumDisplayMonitors(NULL, NULL, monitorEnumProc, (LPARAM)this);
	return !_displays.empty();
}

const std::vector<Display>& ScreenEnumerator::displays() const
{
	return _displays;
}

int ScreenEnumerator::screen(const Rect& screenRect) const
{
	for (unsigned int i = 0; i < _displays.size(); i++)
	{
		if (screenRect == _displays[i].rect)
			return i;
	}
	return -1;
}

BOOL CALLBACK ScreenEnumerator::monitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM data)
{
	ScreenEnumerator* instance = reinterpret_cast<ScreenEnumerator*>(data);
	if (instance != NULL)
	{
		Rect screenRect = Rect::cornered(rect->left, rect->top, rect->right, rect->bottom);
		Display display = Display(hMonitor, screenRect);
		instance->_displays.push_back(display);
	}
	return TRUE;
}

#endif