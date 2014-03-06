#pragma once

#ifdef WIN32

#include "igrabber.h"
#include <Windows.h>

namespace dz
{
	class Grabber_Win32 : public IGrabber
	{
		public:
			Grabber_Win32();

			/// Enables cursor grabbing
			virtual void setEnableGrabCursor(bool enable);

			static void grabCursor(const Rect& rect, HDC hdc);

		protected:
			bool _grabMouseCursor;
	};
}

#endif
