#pragma once

#ifdef MAC_OSX

#include "IDesktopTools.h"
#include "Grabber.h"

#include <ApplicationServices/ApplicationServices.h>

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
			// number of connected displays
			uint32_t m_displayCount;
			Rect m_displaySizes[16];

	};
}

#endif // MAC_OSX

// EOF
