
#pragma once

#ifdef WIN32

#include "DirectXDisplay.h"
#include "Win32Grabber.h"
#include "ScreenEnumerator.h"

#include <d3d9.h>
#include <map>
#include <vector>
#include <windows.h>

namespace dz {

class Window;

class DirectXGrabber : public Win32Grabber
{
public:
	static RECT toRECT(const Rect& rhs);

	DirectXGrabber();
	virtual ~DirectXGrabber();

	void init();
	void deinit();

	int grab(const Rect& captureRect, Buffer* destination);

	int screenCount() const;
	Rect screenResolution(int screen) const;

private:
	void initD3D();
	void initD3DDisplays();

	void enumerateDisplays(IDirect3D9* d3d, const ScreenEnumerator& enumerator);

	void createD3DDevice(int adapter, const Rect& screenRect, IDirect3DDevice9** d3dDevice);
	D3DPRESENT_PARAMETERS createPresentParameters(int adapter, const Rect& rect, HWND hWnd);
	D3DFORMAT findAutoDepthStencilFormat(int adapter, D3DFORMAT backBufferFormat);

	IDirect3D9* _d3d;
	Window*     _window;

	typedef std::vector<DirectXDisplay> DisplayList;
	typedef std::vector<int> AdapterList;

	DisplayList _displays;
	AdapterList _adapters;
};

}

#endif
