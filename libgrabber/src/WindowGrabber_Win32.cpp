#ifdef _WIN32

#include "WindowGrabber_Win32.h"
#include <dzlib/dzexception.h>
#include <libcommon/videoframe.h>
#include <libcommon/videoframepool.h>

#include <cassert>

namespace dz
{
	WindowGrabber_Win32::WindowGrabber_Win32(HWND windowHandle) : m_windowHandle(windowHandle), m_winBitmap(NULL), m_pData(NULL),
		m_desktopTools(IDesktopTools::CreateDesktopTools()), m_areaGrabber(Rect(0, 0, 640, 480)), m_width(0), m_height(0)
	{
		if (IsWindow(m_windowHandle) == FALSE)
			throw exception(strstream() << "WindowGrabber_Win32 given an invalid window handle");

		m_windowDC = GetWindowDC(m_windowHandle);
		if (m_windowDC == NULL)
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - GetWindowDC failed with error code " << GetLastError());

		m_memDC = CreateCompatibleDC(m_windowDC);
		if (m_memDC == NULL)
		{
			ReleaseDC(m_windowHandle, m_windowDC);
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - CreateCompatibleDC failed with error code " << GetLastError());
		}

		m_areaGrabber.SetCaptureRect(GetCaptureRect());
	}
	
	WindowGrabber_Win32::~WindowGrabber_Win32()
	{
		ReleaseDC(m_windowHandle, m_windowDC);

		if (m_winBitmap != NULL)
			DeleteObject(m_winBitmap);

		if (m_memDC)
			DeleteDC(m_memDC);
	}

	void WindowGrabber_Win32::setEnableGrabCursor(bool enable)
	{
	}

	//void WindowGrabber_Win32::GrabWindow(Buffer& destination)
	Rect WindowGrabber_Win32::GetCaptureRect() const
	{
		RECT curWindowRect;
		dz::Rect windowRect;

		if (IsIconic(m_windowHandle) == TRUE)
		{
			if (m_width != 0 && m_height != 0)
				return Rect(m_width, m_height);
			else
				return Rect(320, 240);
		}

		if (GetWindowRect(m_windowHandle, &curWindowRect) == FALSE)
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - GetWindowRect failed with error code " << GetLastError());

		windowRect.x = curWindowRect.left;
		windowRect.y = curWindowRect.top;

		// figure out of the window lies outside of any of the screens, and if yes use the window grab method
		if (OffsetRect(&curWindowRect, -curWindowRect.left, -curWindowRect.top) == FALSE)
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - OffsetRect failed with error code " << GetLastError());

		windowRect.width = curWindowRect.right;
		windowRect.height = curWindowRect.bottom;

		return windowRect;
	}

	//virtual
	VideoFrameHandle WindowGrabber_Win32::GrabVideoFrame()
	{
		// figure out if the window is not the foreground window and if it is not, use the window grabbing mode
		bool bUseFullGrabber = (GetForegroundWindow() == m_windowHandle);

		dz::Rect windowRect = GetCaptureRect();

		if (IsIconic(m_windowHandle) == TRUE)
		{
			auto frame(VideoFramePool::GetInstance().AllocVideoFrame(windowRect.width, windowRect.height));
			if (frame)
				frame->Clear();
			return std::move(frame);
		}

		// if the window is not inside a screen/desktop, we cannot use the area grabber
		if (m_desktopTools->IsInsideAnyScreen(windowRect) == false)
			bUseFullGrabber = false;

		if (bUseFullGrabber)
		{
			m_areaGrabber.SetCaptureRect(windowRect);
			return m_areaGrabber.GrabVideoFrame();
		}

		if (m_width != windowRect.width ||
			m_height != windowRect.height)
		{
			// window size changed
			OnWindowResized(m_windowDC, windowRect.width, windowRect.height);
		}

		//SendMessage(m_windowHandle, WM_PRINT, (WPARAM)m_windowDC, PRF_NONCLIENT | PRF_CLIENT);

		PrintWindow(m_windowHandle, m_memDC, 0);
		//PrintWindow(m_windowHandle, m_memDC, 0);
		//if (::BitBlt(m_memDC, 0, 0, m_width, m_height, m_windowDC, 0, 0, CAPTUREBLT | SRCCOPY) == FALSE)
		//	throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - BitBlt(" << m_width << ", " << m_height << ") failed with error code " << GetLastError());

		uint32_t lineStride = m_width * 4;

		//assert(destination.width == m_width);
		//assert(destination.height == m_height);
		auto frame(VideoFramePool::GetInstance().AllocVideoFrame(m_width, m_height));

		if (frame == nullptr)
			return frame;

		assert(lineStride == frame->GetStride());

		CopyMemory(frame->GetData(), m_pData, lineStride * m_height);
		//CopyMemory(destination.data, m_pData, lineStride * m_height);

		return std::move(frame);
	}

	void WindowGrabber_Win32::OnWindowResized(HDC winDC, uint32_t newWidth, uint32_t newHeight)
	{
		m_width = newWidth;
		m_height = newHeight;

		if (m_winBitmap != NULL)
		{
			//ReleaseDC(m_windowHandle, m_memDC);
			DeleteObject(m_winBitmap);
			m_winBitmap = NULL;
		}

		if (m_memDC != NULL)
		{
			DeleteDC(m_memDC);
			m_memDC = NULL;
		}

		m_memDC = CreateCompatibleDC(m_windowDC);
		if (m_memDC == NULL)
		{
			DeleteDC(m_memDC);
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - CreateCompatibleDC failed with error code " << GetLastError());
		}

		BITMAPINFO bmi;

		// Initialize to 0s.
		ZeroMemory(&bmi, sizeof(bmi));

		// Initialize the header.
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_width;
		bmi.bmiHeader.biHeight = -(int32_t)m_height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		// Create the surface.
		m_winBitmap = CreateDIBSection(winDC, &bmi, DIB_RGB_COLORS, (void**)&m_pData, NULL, 0);
		if (m_winBitmap == NULL)
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - CreateDIBSection(" << m_width << ", " << m_height << ") failed with error code " << GetLastError());

		HGDIOBJ obj = SelectObject(m_memDC, m_winBitmap);
		if (obj == NULL || obj == HGDI_ERROR)
			throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - SelectObject failed with error code " << GetLastError());
	}
}

#endif
