#ifdef WIN32

#include "../Grabber.h"
#include "../null/NullGrabber.h"
#include "BitBltGrabber.h"
#include "DirectXGrabber.h"

#include <iostream>

namespace dz
{
	Grabber* Grabber::create(enum GrabberType type)
	{
		if (type == GT_NULL)
		{
			return new NullGrabber();
		}
		else if (type == GT_DEFAULT)
		{
			return new BitBltGrabber();
		}
		return new DirectXGrabber();
	}

	void Win32Grabber::grabCursor(const Rect& rect, HDC hdc)
	{
		CURSORINFO cursorInfo = { 0 };
		cursorInfo.cbSize = sizeof(CURSORINFO);

		if (GetCursorInfo(&cursorInfo))
		{
			if (cursorInfo.flags == CURSOR_SHOWING)
			{
				ICONINFO iconInfo = { 0 };
				GetIconInfo(cursorInfo.hCursor, &iconInfo);

				POINT pos;
				GetCursorPos(&pos);

				int x = pos.x - rect.x - iconInfo.xHotspot;
				int y = pos.y - rect.y - iconInfo.yHotspot;
				DrawIconEx(hdc, x, y, cursorInfo.hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);

				DeleteObject(iconInfo.hbmColor);
				DeleteObject(iconInfo.hbmMask);
			}
		}
	}

	Win32Grabber::Win32Grabber()
	: _grabMouseCursor(false)
	{
		ScreenEnumerator enumerator;
		enumerator.enumerate();
		_displays = enumerator.displays();
	}

	int Win32Grabber::screenCount () const
	{
		return _displays.size();
	}

	Rect Win32Grabber::screenResolution (int screen) const
	{
		if (screen >= 0 && screen < (int)_displays.size())
		{
			return _displays[screen].rect;
		}
		return Rect();
	}

	Rect Win32Grabber::combinedScreenResolution () const 
	{
		int screenCount = this->screenCount();
		if (screenCount == 0) return Rect();
		Rect r;
		for (int i = 0; i < screenCount; i++)
		{
			r.addToBoundingRect(this->screenResolution(i));
		}
		return r;
	}

	void Win32Grabber::setEnableGrabCursor (bool enable)
	{
		_grabMouseCursor = enable;
	}
}

#endif