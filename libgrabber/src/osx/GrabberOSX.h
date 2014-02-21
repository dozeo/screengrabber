#pragma once
#include "../Grabber.h"

#ifdef MAC_OSX
#include <ApplicationServices/ApplicationServices.h>

namespace dz
{
	class GrabberOSX : public IGrabber
	{
		public:
			GrabberOSX();
			virtual ~GrabberOSX();

			// Implementation of Grabber
			virtual void deinit();
			virtual void setEnableGrabCursor(bool enable = true);
			virtual void grab(const Rect& rect, Buffer * destination);

		private:
			void updateDisplayInformation () const;
			mutable uint32_t mDisplayCount;          // number of connected displays
			mutable CGDirectDisplayID mDisplays[16]; // connected displays
			mutable Rect mDisplaySizes[16];
			bool mEnableGrabCursor;
	};
}

#endif
