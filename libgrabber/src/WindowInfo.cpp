#include "WindowInfo.h"

namespace dz {

#ifdef LINUX

// Not supported

/*static*/ int WindowInfo::populate (std::vector<WindowInfo> * destination, int64_t pid) {
	return Grabber::GE_UNSUPPORTED;
}

/*static*/ WindowInfo WindowInfo::about (int64_t wid) {
	return WindowInfo();
}


#endif

}
