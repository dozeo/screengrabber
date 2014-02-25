#include "IWindowGrabber.h"

namespace dz
{
	void IWindowGrabber::grab(const Rect& rect, Buffer* destination)
	{
		if (destination == NULL)
			throw exception("IWindowGrabber::grab() - Invalid buffer destination (null)");

		GrabWindow(*destination);
	}
}
