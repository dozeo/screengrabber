#ifdef LINUX

#pragma once
#include "../Grabber.h"

#include <X11/Xlib.h>

namespace dz {

/// X11 implementation of Grabber
/// TODO: Multiscreen support
class X11Grabber : public Grabber {
public:

	// Implemntation
	X11Grabber ();
	virtual ~X11Grabber();
	virtual int init ();
	virtual void deinit ();
	virtual int screenCount () const;
	virtual Rect screenResolution (int screen) const;
	virtual Rect combinedScreenResolution () const;
	virtual int grab (const Rect& rect, Buffer * destination);
private:
	int  loadSizesFromRandr ();
	void loadSizeFromOneDiplay (); // fallback method
	Display * mDisplay;
	bool mRandrAvailable;

	int mDisplayCount;
	dz::Rect mDisplaySizes[16];

	XErrorHandler mPreviousHandler; ///< Error handler before we installed our own
};

}

#endif
