
#include "DirectXDisplay.h"

#ifdef WIN32

#include "Win32Grabber.h"
#include "BitBltGrabber.h"
#include "../Math.h"
#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

using namespace dz;

RECT DirectXDisplay::toRECT(const Rect& rhs)
{
	RECT rect;
	rect.left   = rhs.left();
	rect.right  = rhs.right();
	rect.top    = rhs.top();
	rect.bottom = rhs.bottom();
	return rect;
}

int DirectXDisplay::getCursorRect(HBITMAP hbitmap, Rect& rect, bool& isColorIcon)
{
	int result = 0;

	rect = Rect();
	POINT pos = { 0 };
	result = GetCursorPos(&pos);
	if (result != 0) {
		rect.x = pos.x;
		rect.y = pos.y;
	}

	BITMAP bmp = { 0 };
	result = GetObject(hbitmap, sizeof(BITMAP), &bmp);
	if (result != 0) {
		isColorIcon = abs(bmp.bmHeight / bmp.bmWidth) == 2;
		rect.w = bmp.bmWidth;
		rect.h = isColorIcon ? bmp.bmHeight / 2 : bmp.bmHeight;
	}

	return 0;
}

DirectXDisplay::DirectXDisplay(int adapter, const Rect& screenRect)
: _adapter(adapter)
, _screenRect(screenRect)
, _d3dDevice(0)
, _surface(0)
, _hdcDesktop(0)
, _hdcCapture(0)
{
}

DirectXDisplay::~DirectXDisplay()
{
	shutdown();
}

HRESULT DirectXDisplay::init(IDirect3DDevice9* d3dDevice)
{
	assert(d3dDevice != NULL);
	
	_hdcDesktop = GetDC(GetDesktopWindow());
	_hdcCapture = CreateCompatibleDC(_hdcDesktop);

	_d3dDevice = d3dDevice;
	Dimension2 size(_screenRect.w, _screenRect.h);
	HRESULT hr = createOffscreenSurface(size, &_surface);
	return hr;
}

void DirectXDisplay::shutdown()
{
	if (_hdcCapture != 0) {
		DeleteDC(_hdcCapture);
		_hdcCapture = 0;
	}
	if (_hdcDesktop != 0) {
		ReleaseDC(GetDesktopWindow(), _hdcDesktop);
		_hdcDesktop = 0;
	}
	if (_surface != 0) {
		_surface->Release();
		_surface = 0;
	}
	if (_d3dDevice != 0) {
		_d3dDevice->Release();
		_d3dDevice = 0;
	}
}

void DirectXDisplay::resetDevice(D3DPRESENT_PARAMETERS* pp)
{
	if (FAILED(_d3dDevice->Reset(pp)))
		throw exception("DirectXDisplay failed to reset");
}

int DirectXDisplay::adapter() const
{
	return _adapter;
}
	
Rect DirectXDisplay::screenRect() const
{
	return _screenRect;
}

HRESULT DirectXDisplay::grabRect(const Rect& captureRect, int destX, int destY, Buffer* destination, bool showCursor)
{
	assert(_d3dDevice != NULL);

	Rect d3dRect;
	d3dRect.x = captureRect.x - _screenRect.x;
	d3dRect.y = captureRect.y - _screenRect.y;
	d3dRect.w = captureRect.w;
	d3dRect.h = captureRect.h;

	if (d3dRect.w == 0 || d3dRect.h == 0)
		return S_OK;

	HRESULT hr = _d3dDevice->GetFrontBufferData(0, _surface);
	if (hr != S_OK) {
		std::cerr << "Failed to retrieve front buffer!" << std::endl;
		return hr;
	}

	if (showCursor)
		copyCursorToSurface(d3dRect);

	hr = copySurfaceToBuffer(d3dRect, destX, destY, destination);
	if (hr != S_OK) {
		std::cerr << "Failed to copy surface content to buffer!" << std::endl;
		return hr;
	}

	return S_OK;
}

HRESULT DirectXDisplay::copySurfaceToBuffer(const Rect& rect, int destX, int destY, Buffer* destination)
{
	RECT d3dRect = toRECT(rect);
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = _surface->LockRect(&lockedRect, &d3dRect, D3DLOCK_READONLY | D3DLOCK_NOOVERWRITE);
	if (hr != S_OK)
	{
		std::cerr << "Failed to lock rect of surface!" << std::endl;
		return hr;
	}

	int destOffset = destY * destination->rowLength + 4 * destX;
	char* destData = (char*)destination->data + destOffset;
	char* srcData  = (char*)lockedRect.pBits;

	for (int y = 0; y < rect.h; y++)
	{
		memcpy(destData, srcData, rect.w * 4);
		srcData += lockedRect.Pitch;
		destData += destination->rowLength;
	}

	_surface->UnlockRect();

	return S_OK;
}

void DirectXDisplay::copyCursorToSurface(const Rect& rect)
{
	CURSORINFO cursorInfo = { 0 };
	cursorInfo.cbSize = sizeof(CURSORINFO);
	if (!GetCursorInfo(&cursorInfo))
		throw exception("Call to GetCursorInfo failed");

	if (cursorInfo.flags == CURSOR_SHOWING)
	{
		ICONINFO iconInfo = { 0 };
		GetIconInfo(cursorInfo.hCursor, &iconInfo);

		// get cursor properties
		bool isColorIcon = false;
		Rect cursorRect;
		getCursorRect(iconInfo.hbmMask, cursorRect, isColorIcon);
		cursorRect.x -= _screenRect.x + iconInfo.xHotspot;
		cursorRect.y -= _screenRect.y + iconInfo.yHotspot;

		if (cursorRect.intersects(rect))
		{
			BITMAPINFO bmpInfo = { 0 };
			BitBltGrabber::fillBitmapInfo(cursorRect.w, cursorRect.h, bmpInfo);
			HBITMAP hBitmap = CreateCompatibleBitmap(_hdcCapture, cursorRect.w, cursorRect.h);

			// draw cursor into bitmap
			SelectObject(_hdcCapture, hBitmap);
			DrawIcon(_hdcCapture, 0, 0, cursorInfo.hCursor);

			// copy cursor bitmap to surface
			copyBitmap(cursorRect, iconInfo, isColorIcon, rect);

			// release remaining resources
			DeleteObject(hBitmap);
		}

		// it's necessary to release hbmColor and hbmMask, getIconInfo allocates these resources
		DeleteObject(iconInfo.hbmColor);
		DeleteObject(iconInfo.hbmMask);
	}
}

void DirectXDisplay::copyBitmap(const Rect& cursorRect, const ICONINFO& iconInfo, bool isColorIcon, const Rect& rect)
{
	D3DLOCKED_RECT lockedRect;
	RECT d3dRect = toRECT(rect);
	_surface->LockRect(&lockedRect, &d3dRect, D3DLOCK_READONLY | D3DLOCK_NOOVERWRITE);

	HDC hdcMask  = CreateCompatibleDC(_hdcCapture);
	HDC hdcColor = CreateCompatibleDC(_hdcCapture);
	SelectObject(hdcMask,  iconInfo.hbmMask);
	SelectObject(hdcColor, iconInfo.hbmColor);

	// a color icon holds AND & XOR mask in hbmMask attribute, where the bitmap is divided into two 32x32 blocks
	// the upper 32x32 block is the AND mask, the lower the XOR mask
	// if it's a black and white icon, the AND mask (from hbmMask) defines the color to be rendererd or not
	Rect intersect;
	cursorRect.intersects(rect, &intersect);

	for (int y = intersect.top(); y < intersect.bottom(); y++)
	{
		for (int x = intersect.left(); x < intersect.right(); x++)
		{
			int bmpX = x - intersect.x;
			int bmpY = y - intersect.y;

			unsigned int mask = GetPixel(hdcMask, bmpX, bmpY);
			unsigned int color = GetPixel(hdcColor, bmpX, bmpY);

			uint32_t* buffer = (uint32_t*)((uint8_t*)lockedRect.pBits + x * 4 + lockedRect.Pitch * y);

			if (isColorIcon) 
			{
				unsigned int orMask = GetPixel(hdcMask, bmpX, bmpY + cursorRect.h);
				*buffer &= (mask & color) ^ orMask;
			}
			else if (mask != 0xffffff)
			{
				*buffer = color;
			}
		}
	}

	DeleteDC(hdcMask);
	DeleteDC(hdcColor);

	_surface->UnlockRect();
}

HRESULT DirectXDisplay::createOffscreenSurface(const Dimension2& size, IDirect3DSurface9** surface)
{
	// unfortunately the front buffer requires D3DFMT_A8R8G8B
	// IDirect3DSurface9->GetDC only works with D3DFMT_X8R8G8B8, cannot be used therefore
	HRESULT hr = _d3dDevice->CreateOffscreenPlainSurface(
		size.width,
		size.height,
		D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,
		surface,
		0);
	return hr;
}

#endif
