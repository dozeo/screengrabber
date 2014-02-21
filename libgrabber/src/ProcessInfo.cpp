#ifdef LINUX

#include "ProcessInfo.h"

namespace dz
{
	bool ProcessInfo::populate (std::vector<ProcessInfo> * destination)
	{
		return false;
	}

	ProcessInfo ProcessInfo::about (int64_t pid) 
	{
		return ProcessInfo();
	}
}

#endif
