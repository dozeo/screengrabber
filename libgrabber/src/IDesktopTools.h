#pragma once

#include <cstdint>
#include <libcommon/dzrect.h>

namespace dz
{
	class IDesktopTools
	{
		public:
			IDesktopTools() { }
			virtual ~IDesktopTools() { }

			static IDesktopTools* CreateDesktopTools();

			virtual uint32_t GetScreenCount() const = 0;
			virtual Rect GetScreenResolution(uint32_t screen) const = 0;
			virtual Rect GetCombinedScreenResolution() const = 0;
	};
}