#include "NullGrabber.h"
#include <assert.h>

namespace dz {

NullGrabber::NullGrabber () {

}

NullGrabber::~NullGrabber() {

}

int NullGrabber::init () {
	return 0;
}

void NullGrabber::deinit () {

}

int NullGrabber::screenCount () const {
	return 2;
}

Rect NullGrabber::screenResolution (int screen) const {
	if (screen == 0) return Rect (0,0, 1280, 800);
	if (screen == 1) return Rect (1280, 0, 1920, 1080);
	return Rect ();
}
    
Rect NullGrabber::combinedScreenResolution () const {
    Rect box;
    for (int i = 0; i < screenCount(); i++) {
    	box.addToBoundingRect(screenResolution (i));
    }
    return box;
}

int NullGrabber::grab(const Rect& rect, Buffer * destination) {
	assert (rect.w >= 0);
	assert (rect.h >= 0);
	// Qt defines them this way: 0xAARRGGBB;
	// This is internally BB, GG, RR, AA (little endian)
    // in OSX: kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little
	int32_t blue = 0x000000ff;
	Rect first    = screenResolution(0);
	Rect second   = screenResolution(1);
	for (int x = 0; x < dz::minimum (destination->width, rect.w); x++) {
		for (int y = 0; y < dz::minimum (destination->height, rect.h); y++) {
			int rx = rect.x + x;
			int ry = rect.y + y;
			if (first.contains (rx,ry) || second.contains (rx,ry)){
				int32_t * pos = (int32_t* ) (destination->data + y * (destination->rowLength) + x * 4);
				*pos = blue;
			}
		}
	}

	return 0;
}

}
