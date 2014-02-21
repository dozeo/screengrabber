#pragma once

#ifdef WIN32

#include "../Grabber.h"
#include <Windows.h>

namespace dz
{
	class Win32Grabber : public IGrabber
	{
		public:
			Win32Grabber();

			/// Unitializes it again
			virtual void deinit() = 0;

			/// Enables cursor grabbing
			virtual void setEnableGrabCursor(bool enable);

			/// Grab something into the destination buffer
			virtual void grab(const Rect& rect, Buffer * destination) = 0;

			static void grabCursor(const Rect& rect, HDC hdc);

		protected:
			bool _grabMouseCursor;
	};
}

#endif
