#if 0

#include "DirectXGrabber.h"

#ifdef WIN32

#include "Window.h"
#include "../Math.h"

#include <assert.h>
#include <d3d9.h>
#include <algorithm>
#include <vector>

#include <dzlib/dzexception.h>
#include <cstdint>

using namespace dz;

DirectXGrabber::DirectXGrabber() : m_desktopTools(IDesktopTools::CreateDesktopTools())
{
	_window = new Window();

	_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (_d3d == NULL)
	{
		delete _window;
		throw exception(strstream() << "Direct3DCreate9(" << D3D_SDK_VERSION << ") failed to initialize");
	}

	ScreenEnumerator enumerator;
	if (!enumerator.enumerate())
	{
		_d3d->Release();
		delete _window;
		throw exception(strstream() << "Failed to enumerate displays");
	}

	enumerateDisplays(_d3d, enumerator);

	for (uint32_t i = 0; i < m_desktopTools->GetScreenCount(); i++)
	{
		DirectXDisplay& display = _displays[i];
		IDirect3DDevice9* d3dDevice;
		createD3DDevice(display.adapter(), display.screenRect(), &d3dDevice);
		_displays[i].init(d3dDevice);
	}

	// each CreateDevice calls makes the previous one unusable, therefore a reset is necessary
	if (_displays.size() > 1)
	{
		for (unsigned int i = 0; i < _displays.size(); i++)
		{
			DirectXDisplay& display = _displays[i];
			D3DPRESENT_PARAMETERS pp = createPresentParameters(display.adapter(), display.screenRect(), _window->getHWnd());
			
			try
			{
				display.resetDevice(&pp);
			}
			catch (...)
			{
				_d3d->Release();
				delete _window;
				throw;
			}
		}
	}
}

DirectXGrabber::~DirectXGrabber()
{
	deinit();
}

void DirectXGrabber::deinit()
{
	if (_d3d != 0)
	{
		_d3d->Release();
		_d3d = 0;
	}
	if (_window != 0)
	{
		delete _window;	
		_window = 0;
	}
	_displays.clear();
}

void DirectXGrabber::grab(const Rect& captureRect, Buffer* destination)
{
	assert(destination != 0);

	for (unsigned int i = 0; i < _displays.size(); i++)
	{
		DirectXDisplay& display = _displays[i];
		const Rect& displayRect = display.screenRect();

		Rect intersect;
		if (captureRect.intersects(displayRect, &intersect))
		{
			Rect rect(intersect);
			rect.w = intersect.w;
			rect.h = intersect.h;
			int x = intersect.x - captureRect.x;
			int y = intersect.y - captureRect.y;
			display.grabRect(rect, x, y, destination, _grabMouseCursor);
		}
	}
}

void DirectXGrabber::enumerateDisplays(IDirect3D9* d3d, const ScreenEnumerator& enumerator)
{
	assert(d3d != 0);

	_displays.clear();
	for (unsigned int adapter = 0; adapter < d3d->GetAdapterCount(); adapter++)
	{
		HMONITOR hMonitor = d3d->GetAdapterMonitor(adapter);

		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo(hMonitor, &monitorInfo) == TRUE)
		{
			const RECT& monitorRect = monitorInfo.rcMonitor;

			Rect rect;
			rect.x = monitorRect.left;
			rect.y = monitorRect.top;
			rect.w = monitorRect.right - monitorRect.left;
			rect.h = monitorRect.bottom - monitorRect.top;

			int screen = enumerator.screen(rect);
			if (screen != -1)
			{
				_displays.push_back(DirectXDisplay(adapter, rect));
			}
		}
	}
}

void DirectXGrabber::createD3DDevice(int adapter, const Rect& screenRect, IDirect3DDevice9** d3dDevice)
{
	assert(_d3d != NULL);
	assert(_window != NULL);

	Rect windowRect(0, 0, screenRect.w, screenRect.h);
	D3DPRESENT_PARAMETERS pp = createPresentParameters(adapter, windowRect, _window->getHWnd());

	D3DDEVTYPE devType = D3DDEVTYPE_HAL;
	DWORD createFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	HRESULT hr = _d3d->CreateDevice(
		adapter,
		devType,
		_window->getHWnd(),
		createFlags,
		&pp,
		d3dDevice);

	if (FAILED(hr))
		throw exception(strstream() << "DirectX CreateDevice failed");
}

D3DPRESENT_PARAMETERS DirectXGrabber::createPresentParameters(int adapter, const Rect& rect, HWND hWnd)
{
	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(params));

	D3DDISPLAYMODE mode;
	_d3d->GetAdapterDisplayMode(adapter, &mode);

	params.BackBufferCount            = 1;
	params.BackBufferWidth            = rect.w;
	params.BackBufferHeight           = rect.h;
	params.BackBufferFormat           = mode.Format;
	params.Flags                      = 0;
	params.hDeviceWindow              = hWnd;
	params.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	params.Windowed                   = true;
	params.MultiSampleType            = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality         = 0;
	params.FullScreen_RefreshRateInHz = 0;
	params.EnableAutoDepthStencil     = true;
	params.AutoDepthStencilFormat     = findAutoDepthStencilFormat(adapter, mode.Format);
	params.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	return params;
}

D3DFORMAT DirectXGrabber::findAutoDepthStencilFormat(int adapter, D3DFORMAT backBufferFormat)
{
	std::vector<D3DFORMAT> depthFormats;
	depthFormats.push_back(D3DFMT_D32);
	depthFormats.push_back(D3DFMT_D24S8);
	depthFormats.push_back(D3DFMT_D24X8);
	depthFormats.push_back(D3DFMT_D16);
	depthFormats.push_back(D3DFMT_D15S1);

	D3DDEVTYPE devType = D3DDEVTYPE_HAL;

	for each (D3DFORMAT depthFormat in depthFormats)
	{
		HRESULT hr = _d3d->CheckDepthStencilMatch(adapter, devType, backBufferFormat, backBufferFormat, depthFormat);
		if (hr == D3D_OK)
		{
			return depthFormat;
		}
	}
	return D3DFMT_UNKNOWN;
}
#endif


#endif // 0