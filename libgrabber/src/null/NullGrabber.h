#pragma once

#include "../igrabber.h"

namespace dz
{
	/// Dummy implementation of Grabber
	class NullGrabber : public IGrabber
	{
		public:
			NullGrabber ();
			virtual ~NullGrabber();

			virtual void SetCaptureRect(Rect capture) { m_captureRect = capture; }

			// fake one
			Rect screenResolution(int screen) const;

			virtual VideoFrameHandle GrabVideoFrame();
			virtual void setEnableGrabCursor(bool enable = true) { }

		private:
			Rect m_captureRect;
	};
}
