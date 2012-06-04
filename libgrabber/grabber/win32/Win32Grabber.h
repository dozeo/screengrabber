#pragma once

#ifdef WIN32

#include "../Grabber.h"
#include "ScreenEnumerator.h"

namespace dz {

class Win32Grabber : public Grabber {
public:
	static int grabCursor (const Rect& rect, HDC hdc);

	Win32Grabber();

	/// Initializes the grabber, returns 0 on success
	virtual int init () = 0;

	/// Unitializes it again
	virtual void deinit () = 0;

	/// Screen count
	virtual int screenCount () const;

	/// Screen resolution
	virtual Rect screenResolution (int screen) const;

	/// Resolution of all Screens
	virtual Rect combinedScreenResolution () const;

	/// Enables cursor grabbing
	virtual int setEnableGrabCursor (bool enable);

	/// Grab something into the destination buffer
	virtual int grab (const Rect& rect, Buffer * destination) = 0;

private:
	std::vector<Display> _displays;

protected:
	bool _grabMouseCursor;
};

}

#endif