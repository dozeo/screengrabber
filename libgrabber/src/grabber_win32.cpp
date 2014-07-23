#ifdef WIN32

#include "igrabber.h"
#include "grabber_null.h"
#include "bitbltgrabber_win32.h"
#include "WindowGrabber_Win32.h"

#include <iostream>

namespace dz
{
	//static
	IGrabber* IGrabber::CreateGrabber(const GrabberOptions& options)
	{
		IGrabber* grabber = nullptr;

		int64_t windowId = options.m_grabWindowId;
		if (windowId != -1)
			grabber = new WindowGrabber_Win32((HWND)windowId);
		else if (options.m_grabberType == GrabberType::Null)
			grabber = new NullGrabber();
		else
		{
			Rect captureRect = options.m_grabRect;
			grabber = new BitBltGrabber_Win32(captureRect);
		}

		grabber->setEnableGrabCursor(options.m_grabCursor);

		return grabber;
	}

	void Grabber_Win32::grabCursor(const Rect& rect, HDC hdc)
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

	Grabber_Win32::Grabber_Win32()
	: _grabMouseCursor(false)
	{

	}

	void Grabber_Win32::setEnableGrabCursor(bool enable)
	{
		_grabMouseCursor = enable;
	}
}

#endif