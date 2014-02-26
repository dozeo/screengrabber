#if 0

#pragma once

#ifdef WIN32

#include "DirectXDisplay.h"
#include "Win32Grabber.h"
#include "ScreenEnumerator.h"

#include <d3d9.h>
#include <map>
#include <vector>
#include <windows.h>

#include "../IDesktopTools.h"

#include <boost/scoped_ptr.hpp>

namespace dz
{
	class Window;

	class DirectXGrabber : public Win32Grabber
	{
		public:
			static RECT toRECT(const Rect& rhs);

			DirectXGrabber();
			virtual ~DirectXGrabber();

			virtual void grab(const Rect& captureRect, Buffer* destination);

		private:
			void enumerateDisplays(IDirect3D9* d3d, const ScreenEnumerator& enumerator);

			void createD3DDevice(int adapter, const Rect& screenRect, IDirect3DDevice9** d3dDevice);
			D3DPRESENT_PARAMETERS createPresentParameters(int adapter, const Rect& rect, HWND hWnd);
			D3DFORMAT findAutoDepthStencilFormat(int adapter, D3DFORMAT backBufferFormat);

			IDirect3D9* _d3d;
			Window* _window;

			typedef std::vector<DirectXDisplay> DisplayList;
			typedef std::vector<int> AdapterList;

			DisplayList _displays;

			boost::scoped_ptr<dz::IDesktopTools> m_desktopTools;
	};
}

#endif


#endif // 0