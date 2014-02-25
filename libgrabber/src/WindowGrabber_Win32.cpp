#include "WindowGrabber_Win32.h"
#include <dzlib/dzexception.h>

#include <cassert>

namespace dz
{
	WindowGrabber_Win32::WindowGrabber_Win32(HWND windowHandle) : m_windowHandle(windowHandle), m_winBitmap(NULL), m_pData(NULL)
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
	}
	
	WindowGrabber_Win32::~WindowGrabber_Win32()
	{
		ReleaseDC(m_windowHandle, m_windowDC);

		if (m_winBitmap != NULL)
			DeleteObject(m_winBitmap);

		if (m_memDC)
			DeleteDC(m_memDC);
	}

	//static 
	IWindowGrabber* IWindowGrabber::CreateWindowGrabber(int64_t windowId)
	{
		HWND windowHandle = (HWND)windowId;
		return new WindowGrabber_Win32(windowHandle);
	}

	void WindowGrabber_Win32::deinit()
	{
	}

	void WindowGrabber_Win32::setEnableGrabCursor(bool enable)
	{
	}

	void WindowGrabber_Win32::GrabWindow(Buffer& destination)
	{
		RECT curWindowRect;

		try
		{
			if (GetWindowRect(m_windowHandle, &curWindowRect) == FALSE)
				throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - GetWindowRect failed with error code " << GetLastError());

			if (OffsetRect(&curWindowRect, -curWindowRect.left, -curWindowRect.top) == FALSE)
				throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - OffsetRect failed with error code " << GetLastError());

			if (m_width != curWindowRect.right ||
				m_height != curWindowRect.bottom)
			{
				// window size changed
				m_width = curWindowRect.right;
				m_height = curWindowRect.bottom;
				OnWindowResized(m_windowDC, m_width, m_height);
			}

			if (::BitBlt(m_memDC, 0, 0, m_width, m_height, m_windowDC, 0, 0, SRCCOPY) == FALSE)
				throw exception(strstream() << "WindowGrabber_Win32::GrabWindow() - BitBlt(" << m_width << ", " << m_height << ") failed with error code " << GetLastError());

			uint32_t lineStride = m_width * 4;

			//assert(destination.width == m_width);
			//assert(destination.height == m_height);

			CopyMemory(destination.data, m_pData, lineStride * m_height);
		}
		catch (...)
		{
			throw;
		}
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
		bmi.bmiHeader.biHeight = -m_height;
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