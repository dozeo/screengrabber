#pragma once

#ifdef WIN32

#include "../Grabber.h"
#include "../Dimension.h"
#include "../Rect.h"
#include "../platform.h"

#include <d3d9.h>

namespace dz {

class DirectXDisplay {
public:
	static RECT toRECT(const Rect& rhs);
	static int getCursorRect(HBITMAP hbitmap, Rect& rect, bool& isColorIcon);

	DirectXDisplay(int adapter, const Rect& screenRect);
	virtual ~DirectXDisplay();

	HRESULT init(IDirect3DDevice9* d3dDevice);
	void shutdown();
	void resetDevice(D3DPRESENT_PARAMETERS* pp);

	int adapter() const;
	Rect screenRect() const;

	void grabRect(const Rect& intersectRect, int destX, int destY, Buffer* destination, bool showCursor);

private:
	void copySurfaceToBuffer(const Rect& rect, int destX, int destY, Buffer* destination);
	void copyCursorToSurface(const Rect& rect);
	void copyBitmap(const Rect& cursorRect, const ICONINFO& iconInfo, bool isColorIcon, const Rect& rect);

	HRESULT initD3DDevice();
	HRESULT createOffscreenSurface(const Dimension2& size, IDirect3DSurface9** surface);

	int _adapter;
	Rect _screenRect;

	IDirect3DDevice9* _d3dDevice;
	IDirect3DSurface9* _surface;

	HDC _hdcDesktop;
	HDC _hdcCapture;
};

}

#endif
