#ifdef _WIN32

#pragma once

#include "IWindowGrabber.h"

#include <windows.h>

namespace dz
{
	class WindowGrabber_Win32 : public IWindowGrabber
	{
		public:
			WindowGrabber_Win32(HWND windowHandle);
			virtual ~WindowGrabber_Win32();

			virtual void deinit();
			virtual void setEnableGrabCursor(bool enable = true);

			/// Grab something into the destination buffer
			virtual void GrabWindow(Buffer& destination);

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
