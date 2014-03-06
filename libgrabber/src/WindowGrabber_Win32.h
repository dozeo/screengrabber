#ifdef _WIN32

#pragma once

#include <windows.h>

#include "igrabber.h"

namespace dz
{
	class WindowGrabber_Win32 : public IGrabber
	{
		public:
			WindowGrabber_Win32(HWND windowHandle);
			virtual ~WindowGrabber_Win32();

			virtual void SetCaptureRect(Rect capture) { }

			virtual void setEnableGrabCursor(bool enable = true);

			/// Grab something into the destination buffer
			virtual VideoFrameHandle GrabVideoFrame();

		private:
			void OnWindowResized(HDC winDC, uint32_t newWidth, uint32_t newHeight);
			HWND m_windowHandle;
			HDC m_windowDC;
			HDC m_memDC;
			HBITMAP m_winBitmap;
			uint32_t m_width, m_height;
			uint8_t* m_pData;
	};
}

#endif // _WIN32
