#ifdef _WIN32

#pragma once

#include "IDesktopTools.h"
#include "igrabber.h"
#include "screen_enumerator_win32.h"

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
			virtual bool IsInsideAnyScreen(const Rect& otherRect) const;

		private:
			std::vector<Display> m_displays;
	};
}

#endif
// EOF