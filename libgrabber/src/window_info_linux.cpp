#ifdef LINUX

#include "WindowInfo.h"

namespace dz
{
	void WindowInfo::populate(std::vector<WindowInfo> * destination, int64_t pid)
	{
	}

	WindowInfo WindowInfo::about (int64_t wid)
	{
		return WindowInfo();
	}
}

#endif
