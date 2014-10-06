#include "IDesktopTools.h"
#include <slog/slog.h>

#ifdef _WIN32
	#include "DesktopTools_Win32.h"
#endif // _WIN32

#ifdef MAC_OSX
	#include "DesktopTools_OSX.h"
#endif // MAC_OSX

namespace dz
{
	//static
	IDesktopTools* IDesktopTools::CreateDesktopTools()
	{
		#ifdef _WIN32
			return new DesktopTools_Win32();
		#endif // _WIN32

		#ifdef MAC_OSX
			return new DesktopTools_OSX();
		#endif // MAC_OSX

		throw exception(strobj() << "Failed to instantiate the desktop tools for this platform!");
	}
}

// EOF
