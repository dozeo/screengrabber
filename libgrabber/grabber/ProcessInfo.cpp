#include "ProcessInfo.h"

namespace dz {

#ifdef LINUX

// Not supported

/*static*/ int ProcessInfo::populate (std::vector<ProcessInfo> * destination) {
	return Grabber::GE_UNSUPPORTED;
}

/*static*/ ProcessInfo ProcessInfo::about (int64_t pid) {
	return ProcessInfo();
}

#endif

}
