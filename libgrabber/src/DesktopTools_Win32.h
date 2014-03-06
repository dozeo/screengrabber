#ifdef _WIN32

#pragma once

#include "IDesktopTools.h"
#include "igrabber.h"
#include "win32/ScreenEnumerator.h"

namespace dz
{
	class DesktopTools_Win32 : public IDesktopTools
	{
		public:
			DesktopTools_Win32();
			virtual ~DesktopTools_Win32();

			virtual uint32_t GetScreenCount() const;
			virtual Rect GetScreenResolution(uint32_t screen) const;
			virtual Rect GetCombinedScreenResolution() const;

		private:
			std::vector<Display> m_displays;
	};
}

#endif
// EOF