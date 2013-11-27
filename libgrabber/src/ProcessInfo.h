#pragma once

#include "Grabber.h"
#include <string>
#include <vector>

namespace dz
{
	/// Contains information about one process
	struct ProcessInfo
	{
		ProcessInfo() : pid(0){}
		bool valid () const { return pid != 0;}
		int64_t pid;		///< process id, 0 is invalid
		std::string exec;	///< Executable, if reliable derminable

		/// Populate all running applications
		static int populate (std::vector<ProcessInfo> * destination);

		/// Returns information about one specific process, returns invalid info if not supported / non existant
		static ProcessInfo about (int64_t pid);
	};
}
