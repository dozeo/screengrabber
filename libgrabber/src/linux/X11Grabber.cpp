#ifdef LINUX

#include "X11Grabber.h"
#include <iostream>
#include <assert.h>

#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <string.h>

namespace dz {

static int x11ErrorHandler (Display * d, XErrorEvent * e)
{
	char text[128] = "";
	XGetErrorText(d, e->error_code, text, (sizeof text) - 1);
	fprintf (stderr, "Error: X11 Error %d (%s),%d Request:%d\n", e->error_code, text, e->minor_code, e->request_code);
	return 0;
}

X11Grabber::X11Grabber () : mRandrAvailable(false)
{
	mDisplay = XOpenDisplay (NULL);
	if (!mDisplay)
		throw exception(strstream() << "Could not open XDisplay");

	mDisplayCount = 1; // default
	int eventBase;
	int errorBase;
	Bool suc = XRRQueryExtension (mDisplay, &eventBase, &errorBase);

	if (suc)
	{
		mRandrAvailable = true;
		int res = loadSizesFromRandr ();
		if (res)
			loadSizeFromOneDiplay();
	}
	else
	{
		std::cerr << "No xrandr" << std::endl;
		loadSizeFromOneDiplay();
	}

	mPreviousHandler = XSetErrorHandler(&x11ErrorHandler);
}

X11Grabber::~X11Grabber()
{
	XCloseDisplay(mDisplay);
	XSetErrorHandler(mPreviousHandler);
}

int X11Grabber::screenCount() const
{
	return mDisplayCount;
}

Rect X11Grabber::screenResolution(int screen) const
{
	if (screen < 0 || screen > mDisplayCount) return Rect();
	return mDisplaySizes[screen];
}

Rect X11Grabber::combinedScreenResolution () const
{
	Rect result;
	result.w = DisplayWidth  (mDisplay, 0);
	result.h = DisplayHeight (mDisplay, 0);
	return result;
}

/// Clips rectangle into the given window
static Rect clipRect(const Rect & rect, Display * display, const Window& root)
{
	Window rootReturn;
	int x,y;
	unsigned int w;
	unsigned int h;
	unsigned int bw;
	unsigned int dw;
	Status ret = XGetGeometry(display, root, &rootReturn, &x, &y, &w, &h, &bw, &dw);
	if (!ret) return Rect();
	Rect win (x,y,w,h);
	Rect inter;
	rect.intersects(win, &inter);
	return inter;
}

void X11Grabber::grab(const Rect& rect, Buffer * destination)
{
	int screen = 0; // no multi window suppot
	Window root = RootWindow (mDisplay, screen);

	Rect clippedRect = clipRect (rect, mDisplay, root);
	if (clippedRect.empty())
		return 0; // nothing to grab
	Buffer subBuffer;
	subBuffer.initAsSubBufferFrom(
			destination,
			clippedRect.x - rect.x,
			clippedRect.y - rect.y,
			clippedRect.w,
			clippedRect.h);

	XImage * image = XGetImage (mDisplay, root, clippedRect.x, clippedRect.y, clippedRect.w, clippedRect.h, XAllPlanes(), ZPixmap);
	if (!image) {
		std::cerr << "Could not get image" << std::endl;
		return 1;
	}
	assert (image->bits_per_pixel == 32);
	if (image->bits_per_pixel != 32) {
		std::cerr << "Wrong BPP count!" << std::endl;
		return 2;
	}
	for (int y = 0; y < clippedRect.h; y++) {
		void * source = image->data + y * image->bytes_per_line;
		void * target = subBuffer.data + y * subBuffer.rowLength;
		memcpy (target, source, rect.w * 4);
	}
	XDestroyImage (image);
	return 0;
}

int X11Grabber::loadSizesFromRandr () {
	XRRScreenResources * res = XRRGetScreenResources (mDisplay, RootWindow (mDisplay, 0));
	if (!res) {
		// fallback
		std::cerr << "Could not get screen configuration, doing all into one virtual screen" << std::endl;
		return 1;
	}

	int maxDisplays = sizeof (mDisplaySizes) / sizeof (dz::Rect);
	int num = res->ncrtc;
	if (num > maxDisplays) {
		std::cerr << "More displays " << num << " than supported " << maxDisplays << std::endl;
		num = maxDisplays;
	}
	for (int i = 0; i < num; i++) {
		XRRCrtcInfo * crtcInfo = XRRGetCrtcInfo (mDisplay, res, res->crtcs[i]);
		mDisplaySizes[i] = Rect (crtcInfo->x, crtcInfo->y, crtcInfo->width, crtcInfo->height);
		XRRFreeCrtcInfo(crtcInfo);
	}
	mDisplayCount = num;
	XRRFreeScreenResources(res);
	return 0;
}

void X11Grabber::loadSizeFromOneDiplay () {
	mDisplayCount = 1;
	Rect result;
	result.w = DisplayWidth  (mDisplay, 0);
	result.h = DisplayHeight (mDisplay, 0);
	mDisplaySizes[0] = result;
}


}

#endif
