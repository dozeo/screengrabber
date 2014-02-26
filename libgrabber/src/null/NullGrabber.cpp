#include "NullGrabber.h"
#include <assert.h>
#include <string.h>

namespace dz
{
	NullGrabber::NullGrabber () {

	}

	NullGrabber::~NullGrabber() {

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

	void fillWithColor (Buffer * destination, int32_t color) {
		for (int y = 0; y < destination->height; y++) {
			int32_t * line = (int32_t*) (destination->data + y * destination->rowLength);
			for (int x = 0; x < destination->width; x++)
			{
				uint32_t rintensity = rand() % 255;
				uint32_t gintensity = rand() % 255;
				uint32_t bintensity = rand() % 255;
				uint32_t intensity = (0xff << 24) | (rintensity << 16) | (gintensity << 8) | (bintensity);
				*(line + x) = intensity;
			}
		}
	}

	void NullGrabber::grab(const Rect& rect, Buffer * destination)
	{
		assert (rect.w >= 0);
		assert (rect.h >= 0);
		// Qt defines them this way: 0xAARRGGBB;
		// This is internally BB, GG, RR, AA (little endian)
		// in OSX: kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little
		int32_t blue = 0x000000ff;
		Rect first    = screenResolution(0);
		Rect second   = screenResolution(1);

		Rect cut;
		if (rect.intersects(first, &cut)) {
			Buffer part;
			part.initAsSubBufferFrom(destination, cut.x, cut.y, cut.w, cut.h);
			fillWithColor (&part, blue);
		}
		if (rect.intersects(second, &cut)){
			Buffer part;
			part.initAsSubBufferFrom(destination, cut.x - rect.x, cut.y - rect.y, cut.w, cut.h);
			fillWithColor (&part, blue);
		}
	}
}
