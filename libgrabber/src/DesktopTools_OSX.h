#pragma once

#ifdef MAC_OSX

#include "IDesktopTools.h"
#include "Grabber.h"

namespace dz
{
	class DesktopTools_OSX : public IDesktopTools
	{
		public:
			DesktopTools_OSX();
			virtual ~DesktopTools_OSX();

			virtual uint32_t GetScreenCount() const;
			virtual Rect GetScreenResolution(uint32_t screen) const;
			virtual Rect GetCombinedScreenResolution() const;

		private:
			std::vector<Display> m_displays;

			// number of connected displays
			uint32_t mDisplayCount;

			// connected displays
			CGDirectDisplayID mDisplays[16];

			Rect mDisplaySizes[16];

	};
}

#endif // MAC_OSX

// EOF
