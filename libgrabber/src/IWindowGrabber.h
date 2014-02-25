#pragma once

#include "Grabber.h"
#include <dzlib/dzexception.h>

#include <cstdint>

namespace dz
{
	class IWindowGrabber : public IGrabber
	{
		public:
			IWindowGrabber() { }
			virtual ~IWindowGrabber() { }

			static IWindowGrabber* CreateWindowGrabber(int64_t windowId);

			virtual void grab(const Rect& rect, Buffer* destination);

			virtual void GrabWindow(Buffer& destination) = 0;
	};
}