#pragma once

#include <cstdint>
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
			virtual ~Grabber() {}

			/// Returns a new grabber
			static Grabber* create(GrabberType type = GT_DEFAULT);

			/// Initializes the grabber, returns 0 on success
			virtual void init() = 0;

			/// Unitializes it again
			virtual void deinit() = 0;

			/// Screen count
			virtual int screenCount() const = 0;

			/// Screen resolution
			virtual Rect screenResolution(int screen) const = 0;

			/// Resolution of all Screens
			virtual Rect combinedScreenResolution() const = 0;
    
			/// Enables cursor grabbing
			virtual void setEnableGrabCursor(bool enable = true) = 0;

			/// Grab something into the destination buffer
			virtual int grab(const Rect& rect, Buffer * destination) = 0;
	};
}
