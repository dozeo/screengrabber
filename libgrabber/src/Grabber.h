#pragma once

#include <tr1/cstdint>
#include "platform.h"
#include "Rect.h"
#include "Buffer.h"

#include <vector>

namespace dz
{
	enum GrabberType { GT_NULL, GT_DEFAULT, GT_DIRECTX };

	/// Main platform independet interface for grabber
	/// Names:
	/// - screen - The "real" attached screen, 0 is default screen
	class Grabber
	{
		public:
    
			/// Error codes for grabber library
			/// Note: also other error codes may happen
			/// as some platforms just return a dependent int.
			enum GrabberError {
			  GE_OK = 0,
			  GE_INVALID_SCREEN = 1000,
			  GE_COULD_NOT_GRAB = 1001,
			  GE_UNSUPPORTED    = 1002
			};
    
			virtual ~Grabber() {}

			/// Returns a new grabber
			static Grabber* create(GrabberType type = GT_DEFAULT);

			/// Initializes the grabber, returns 0 on success
			virtual int init() = 0;

			/// Unitializes it again
			virtual void deinit() = 0;

			/// Screen count
			virtual int screenCount() const = 0;

			/// Screen resolution
			virtual Rect screenResolution(int screen) const = 0;

			/// Resolution of all Screens
			virtual Rect combinedScreenResolution() const = 0;
    
			/// Enables cursor grabbing
			virtual int setEnableGrabCursor(bool enable = true) { return enable ? GE_UNSUPPORTED : GE_OK; }

			/// Grab something into the destination buffer
			virtual int grab(const Rect& rect, Buffer * destination) = 0;
	};
}
