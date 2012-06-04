#ifdef WIN32
#include <windows.h>
#include <psapi.h> // for iterating processes

#include "../ProcessInfo.h"

namespace dz {

/*static*/ int ProcessInfo::populate (std::vector<ProcessInfo> * destination) {
	destination->clear();

	DWORD processes[4096];
	DWORD needed;
	EnumProcesses (processes, sizeof processes, &needed);
	int count = needed / sizeof (DWORD);
	destination->reserve (count);

	for (int i = 0; i < count; i++) {
		DWORD pid = processes[i];
		if (pid == 0) continue; // ignoring IDLE process
		if (pid == 4) continue; // ignoring Kernel
		HANDLE h = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, pid);
		ProcessInfo info;
		info.pid = pid;
		if (h != NULL) {
			CHAR filename [1024] = "";
			DWORD size = GetProcessImageFileName (h, filename, (sizeof filename) - 1);
			info.exec = filename;
		}
		CloseHandle (h);
		destination->push_back (info);
	}
	return Grabber::GE_OK;
}

/*static*/ ProcessInfo ProcessInfo::about (int64_t pid) {
	HANDLE h = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!h) return ProcessInfo(); // fail

	ProcessInfo info;
	CHAR filename [1024] = "";
	DWORD size = GetProcessImageFileName (h, filename, (sizeof filename) - 1);
	info.exec  = filename;
	info.pid   = pid;
	CloseHandle (h);
	return info;
}


}


#endif
