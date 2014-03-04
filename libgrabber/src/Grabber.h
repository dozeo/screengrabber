#pragma once

#include <cstdint>
#include "platform.h"
#include "Buffer.h"

#include <dzlib/dzexception.h>
#include <libcommon/dzrect.h>

#include <vector>

namespace dz
{
	namespace GrabberType
	{
		enum Enum
		{
			Null,
			Default,
			//DirectX,
			GrabWindow
		};

		std::string ToString(Enum type);
		Enum FromString(const std::string &value);
	}

	/// Main platform independet interface for grabber
	/// Names:
	/// - screen - The "real" attached screen, 0 is default screen
	class IGrabber
	{
		public:
			virtual ~IGrabber() {}

			/// Returns a new grabber
			static IGrabber* create(GrabberType::Enum type = GrabberType::Default);

			/// Enables cursor grabbing
			virtual void setEnableGrabCursor(bool enable = true) = 0;

			/// Grab something into the destination buffer
			virtual void grab(const Rect& rect, Buffer* destination) = 0;
	};
}
