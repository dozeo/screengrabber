#pragma once

#include "../igrabber.h"

#ifdef MAC_OSX
#include <ApplicationServices/ApplicationServices.h>

namespace dz
{
	class GrabberOSX : public IGrabber
	{
		public:
			GrabberOSX();
			virtual ~GrabberOSX();

			virtual void SetCaptureRect(Rect capture);

			// Implementation of Grabber
			virtual void setEnableGrabCursor(bool enable = true);
			virtual VideoFrameHandle GrabVideoFrame();

		private:
			bool mEnableGrabCursor;
	};
}

#endif
