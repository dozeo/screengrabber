#pragma once

#include "Grabber.h"

#include <cstdint>

namespace dz
{
	class IWindowGrabber : public IGrabber
	{
		public:
			IWindowGrabber(int64_t windowId) { }
			virtual ~IWindowGrabber() { }


	};
}