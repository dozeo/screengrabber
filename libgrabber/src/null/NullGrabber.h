#pragma once

#include "../Grabber.h"

namespace dz
{
	/// Dummy implementation of Grabber
	class NullGrabber : public Grabber
	{
		public:
			NullGrabber ();
			virtual ~NullGrabber();

			// Implementation
			virtual void init ();
			virtual void deinit ();
			virtual int screenCount () const;
			virtual Rect screenResolution (int screen) const;
			virtual Rect combinedScreenResolution () const;
			virtual void grab(const Rect& rect, Buffer * destination);
			virtual void setEnableGrabCursor(bool enable = true) { }
	};
}
