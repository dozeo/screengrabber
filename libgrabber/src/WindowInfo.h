#pragma once
#include "Grabber.h"
#include <vector>

namespace dz
{
	/// Contains information about one top level window window
	struct WindowInfo
	{
		WindowInfo () : pid(0), id(0) {}
		bool valid () const { return id > 0; }
		Rect area;			///< Rectangle where the window is at the moment
		int64_t pid;		///< Owning process
		int64_t id;			///< Window id
		std::string title;	///< Window title

		/// Populates all visible windows
		/// If pid is not 0, only windows from a process are being populated
		static void populate (std::vector<WindowInfo> * destination, int64_t pid = 0);

		/// Returns information about one specific window, returns invalid if not supported / non existant
		static WindowInfo about (int64_t wid);
	};
}
