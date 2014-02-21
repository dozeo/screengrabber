#ifdef UNIX
#include "../Grabber.h"
#include "../null/NullGrabber.h"

#ifdef LINUX
#include "../linux/X11Grabber.h"
#endif

#ifdef MAC_OSX
#include "../osx/GrabberOSX.h"
#endif

namespace dz {

Grabber * Grabber::create (enum GrabberType type) {
#ifdef LINUX
	if (type == dz::GT_DEFAULT) return new X11Grabber();
#endif
#ifdef MAC_OSX
    if (type == dz::GT_DEFAULT) return new GrabberOSX();
#endif
	return new NullGrabber ();
}

}

#endif
