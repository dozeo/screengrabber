#ifdef WIN32

#include "bitbltgrabber_win32.h"
#include "igrabber.h"

#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

#include <libcommon/videoframe.h>
#include <libcommon/videoframepool.h>

#define BITS_PER_PIXEL 32

namespace dz
{
	void BitBltGrabber_Win32::fillBitmapInfo(int width, int height, BITMAPINFO& info)
	{
		ZeroMemory(&info, sizeof(BITMAPINFO));
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = width;
		info.bmiHeader.biHeight = -height;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = BITS_PER_PIXEL; // was 32 AH! well I need to change more code for 24 to work
		info.bmiHeader.biSizeImage = 0;
		info.bmiHeader.biCompression = BI_RGB;
	}

	BitBltGrabber_Win32::BitBltGrabber_Win32(Rect capture) : _hdcDesktop(0), _hdcCapture(0), _hBitmap(0), _bmpBuffer(0), m_captureRect(capture)
	{
		_hdcDesktop = GetDC(GetDesktopWindow());
		if (_hdcDesktop == NULL)
			throw exception(strstream() << "GetDC failed - error code is " << GetLastError());

		_hdcCapture = CreateCompatibleDC(_hdcDesktop);
		if (_hdcCapture == NULL)
			throw exception(strstream() << "CreateCompatibleDC failed - error code is " << GetLastError());
	}

	BitBltGrabber_Win32::~BitBltGrabber_Win32()
	{
		ReleaseDC(GetDesktopWindow(), _hdcDesktop);
		DeleteDC(_hdcCapture);

		shutdownBitmap();
		shutdownBitmapBuffer();
	}

	void BitBltGrabber_Win32::SetCaptureRect(Rect capture)
	{
		m_captureRect = capture;
	}

	//virtual 
	VideoFrameHandle BitBltGrabber_Win32::GrabVideoFrame()
	{
		resizeBitmapIfNecessary(m_captureRect.width, m_captureRect.height);

		VideoFrameFormat::Enum format = (BITS_PER_PIXEL == 32) ? VideoFrameFormat::RGBA : VideoFrameFormat::RGB;
		auto frame(VideoFramePool::GetInstance().AllocVideoFrame(m_captureRect.width, m_captureRect.height, format));
		if (frame)
		{
			SelectObject(_hdcCapture, _hBitmap);

			BOOL result = BitBlt(_hdcCapture, 0, 0, m_captureRect.width, m_captureRect.height, _hdcDesktop, m_captureRect.x, m_captureRect.y, SRCCOPY | CAPTUREBLT);
			if (result == FALSE)
				throw exception(strstream() << "Failed to BitBlt with error code " << GetLastError());

			// draw mouse cursor into buffer
			if (_grabMouseCursor)
			{
				grabCursor(m_captureRect, _hdcCapture);
			}
	
			int lines = GetDIBits(_hdcDesktop, _hBitmap, 0, _captureSize.height, _bmpBuffer, &_bmpInfo, DIB_RGB_COLORS);
			if (lines == 0)
				throw exception(strstream() << "Failed to GetDIBits with return " << lines << " and with error code " << GetLastError());

			//copyBitmapToBuffer(_bmpBuffer, m_captureRect.width * (BITS_PER_PIXEL / 8), lines, destination);
			CopyToFrame(frame);
		}

		return std::move(frame);
	}

	void BitBltGrabber_Win32::copyBitmapToBuffer(uint8_t* src, int srcWidth, int lines, Buffer* buffer)
	{
		uint8_t* dest = buffer->data;
		for (int y = 0; y < lines; y++)
		{
			CopyMemory(dest, src, srcWidth);
			dest += buffer->rowLength;
			src += srcWidth;
		}
	}

	void BitBltGrabber_Win32::CopyToFrame(VideoFrameHandle& frame)
	{
		uint8_t* dst = frame->GetData();
		uint8_t* src = _bmpBuffer;
		uint32_t dstStride = frame->GetStride();
		uint32_t srcStride = m_captureRect.width * (BITS_PER_PIXEL / 8);

		for (uint32_t i = 0; i < frame->GetHeight(); i++)
		{
			CopyMemory(dst, src, srcStride);
			dst += dstStride;
			src += srcStride;
		}
	}

	void BitBltGrabber_Win32::resizeBitmapIfNecessary(int width, int height)
	{
		Dimension2 captureSize(width, height);
		if (_captureSize != captureSize)
		{
			initBitmap(captureSize);
			initBitmapBuffer(captureSize);
			_captureSize = captureSize;
		}
	}

	void BitBltGrabber_Win32::initBitmap(const Dimension2& captureSize)
	{
		shutdownBitmap();

		fillBitmapInfo(captureSize.width, captureSize.height, _bmpInfo);
		_hBitmap = CreateCompatibleBitmap(_hdcDesktop, captureSize.width, captureSize.height);
		if (_hBitmap == NULL)
			throw exception(strstream() << "CreateCompatibleBitmap failed with capture size (" << captureSize.width << ", " << captureSize.height << ")");
	}

	void BitBltGrabber_Win32::initBitmapBuffer(const Dimension2& captureSize)
	{
		shutdownBitmapBuffer();

		// TODO: fix this hard coded bytes per pixel value
		uint32_t bufferSize = captureSize.width * 4 * captureSize.height;
		_bmpBuffer = new uint8_t[bufferSize];
	}

	void BitBltGrabber_Win32::shutdownBitmap()
	{
		if (_hBitmap != 0)
		{
			DeleteObject(_hBitmap);
			_hBitmap = 0;
		}
	}

	void BitBltGrabber_Win32::shutdownBitmapBuffer()
	{
		if (_bmpBuffer != 0)
		{
			delete[] _bmpBuffer;
			_bmpBuffer = 0;
		}
	}
}

#endif
