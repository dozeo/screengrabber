#ifdef MAC_OSX

#include "../ProcessInfo.h"
#include <Carbon/Carbon.h>

namespace dz
{
	bool ProcessInfo::populate (std::vector<ProcessInfo> * destination)
	{
		destination->clear ();
	
		ProcessSerialNumber psn = { kNoProcess, kNoProcess };
		while (!GetNextProcess(&psn))
		{
			pid_t pid;
			OSStatus err = GetProcessPID (&psn, &pid);
			if (err)
				continue; // not interested

			ProcessInfo target;
			target.pid = pid;
			FSRef location;
			err = GetProcessBundleLocation(&psn, &location);
			if (err)
				continue; // not interested

			unsigned char path [1024] = "";
			FSRefMakePath(&location, path, (sizeof path) - 1);
			target.exec = (const char*) path;

			destination->push_back (target);
		}

		return true;
	}

/*static*/ ProcessInfo ProcessInfo::about (int64_t pid) {
    ProcessSerialNumber psn;
    OSStatus err = GetProcessForPID((pid_t) pid, &psn);
    if (err) 
        return ProcessInfo ();
    
    FSRef location;
    err = GetProcessBundleLocation (&psn, &location);
    if (err) 
        return ProcessInfo ();
    
    unsigned char path[1024] = "";
    FSRefMakePath (&location, path, (sizeof path) - 1);
    
    ProcessInfo target;
    target.pid  = pid;
    target.exec = (const char*) path; 
    return target;
}
    

}

#endif