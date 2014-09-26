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
			virtual Rect GetCaptureRect() const override { return _capture_rect; }

			// Implementation of Grabber
			virtual void setEnableGrabCursor(bool enable = true);
			virtual VideoFrameHandle GrabVideoFrame();

		private:
			bool mEnableGrabCursor;
			Rect _capture_rect;
	};
}

#endif
