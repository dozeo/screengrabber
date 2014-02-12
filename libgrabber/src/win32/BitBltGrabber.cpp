#include "BitBltGrabber.h"

#ifdef WIN32

#include "../Grabber.h"
#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

namespace dz {

void BitBltGrabber::fillBitmapInfo(int width, int height, BITMAPINFO& info)
{
	ZeroMemory(&info, sizeof(BITMAPINFO));
	info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth       = width;
	info.bmiHeader.biHeight      = -height;
	info.bmiHeader.biPlanes      = 1;
	info.bmiHeader.biBitCount    = 32;
	info.bmiHeader.biSizeImage   = 0;
	info.bmiHeader.biCompression = BI_RGB;
}

BitBltGrabber::BitBltGrabber() : _hdcDesktop(0), _hdcCapture(0), _hBitmap(0), _bmpBuffer(0)
{
}

BitBltGrabber::~BitBltGrabber()
{
	deinit();
}

void BitBltGrabber::init()
{
	_hdcDesktop = GetDC(GetDesktopWindow());
	if (_hdcDesktop == NULL)
		throw exception(strstream() << "GetDC failed - error code is " << GetLastError());

	_hdcCapture = CreateCompatibleDC(_hdcDesktop);
	if (_hdcCapture == NULL)
		throw exception(strstream() << "CreateCompatibleDC failed - error code is " << GetLastError());
}

void BitBltGrabber::deinit()
{
	ReleaseDC(GetDesktopWindow(), _hdcDesktop);
	DeleteDC(_hdcCapture);

	shutdownBitmap();
	shutdownBitmapBuffer();
}

int BitBltGrabber::grab(const Rect& rect, Buffer* destination)
{
	resizeBitmapIfNecessary(rect.w, rect.h);

	SelectObject(_hdcCapture, _hBitmap);

	BOOL result = BitBlt(_hdcCapture, 0, 0, rect.w, rect.h, _hdcDesktop, rect.x, rect.y, SRCCOPY | CAPTUREBLT);
	if (result == FALSE)
	{
		std::cerr << "Failed to BitBlt!" << std::endl;
		return -1;
	}

	// draw mouse cursor into buffer
	if (_grabMouseCursor)
	{
		grabCursor(rect, _hdcCapture);
	}
	
	int lines = GetDIBits(_hdcDesktop, _hBitmap, 0, _captureSize.height, _bmpBuffer, &_bmpInfo, DIB_RGB_COLORS);
	if (lines == 0)
	{
		std::cerr << "Failed to GetDIBits!" << std::endl;
		return -1;
	}

	copyBitmapToBuffer(_bmpBuffer, rect.w * 4, lines, destination);

	return 0;
}

void BitBltGrabber::copyBitmapToBuffer(uint8_t* src, int srcWidth, int lines, Buffer* buffer)
{
	uint8_t* dest = buffer->data;
	for (int y = 0; y < lines; y++)
	{
		CopyMemory(dest, src, srcWidth);
		dest += buffer->rowLength;
		src += srcWidth;
	}
}

void BitBltGrabber::resizeBitmapIfNecessary(int width, int height)
{
	Dimension2 captureSize(width, height);
	if (_captureSize != captureSize)
	{
		initBitmap(captureSize);
		initBitmapBuffer(captureSize);
		_captureSize = captureSize;
	}
}

void BitBltGrabber::initBitmap(const Dimension2& captureSize)
{
	shutdownBitmap();

	fillBitmapInfo(captureSize.width, captureSize.height, _bmpInfo);
	_hBitmap = CreateCompatibleBitmap(_hdcDesktop, captureSize.width, captureSize.height);
}

void BitBltGrabber::initBitmapBuffer(const Dimension2& captureSize)
{
	shutdownBitmapBuffer();

	int bufferSize = ((captureSize.width * 32 + 31) / 32) * 4 * captureSize.height;
	_bmpBuffer = new uint8_t[bufferSize];
}

void BitBltGrabber::shutdownBitmap()
{
	if (_hBitmap != 0)
	{
		DeleteObject(_hBitmap);
		_hBitmap = 0;
	}
}

void BitBltGrabber::shutdownBitmapBuffer()
{
	if (_bmpBuffer != 0)
	{
		delete[] _bmpBuffer;
		_bmpBuffer = 0;
	}
}

}

#endif
