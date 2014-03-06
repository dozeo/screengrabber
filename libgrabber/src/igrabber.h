#pragma once

#include <cstdint>
#include "platform.h"
#include "Buffer.h"

#include <dzlib/dzexception.h>
#include <libcommon/dzrect.h>
//#include <libcommon/grabber_type.h>
#include <libcommon/grabber_options.h>
#include <libcommon/videoframe.h>

#include <vector>

namespace dz
{
	/// Main platform independet interface for grabber
	/// Names:
	/// - screen - The "real" attached screen, 0 is default screen
	class IGrabber
	{
		public:
			virtual ~IGrabber() {}

			static IGrabber* CreateGrabber(const GrabberOptions& options);

			virtual void SetCaptureRect(Rect capture) = 0;

			/// Enables cursor grabbing
			virtual void setEnableGrabCursor(bool enable = true) = 0;

			/// Grab something into the destination buffer
			virtual VideoFrameHandle GrabVideoFrame() = 0;
	};
}
