#ifdef MAC_OSX

#include "GrabberOSX.h"
#include "../DesktopTools_OSX.h"

#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

namespace dz
{
	GrabberOSX::GrabberOSX ()
	{
		mEnableGrabCursor = false;
	}

	GrabberOSX::~GrabberOSX() {}

	IGrabber* IGrabber::create(GrabberType::Enum type)
	{
		switch (type)
		{
			case Null: return new NullGrabber();
			default: return new GrabberOSX();
		}
	}

	void GrabberOSX::setEnableGrabCursor (bool enable)
	{
		mEnableGrabCursor = enable;
	}

	/// Grabs a part of a specific display (in screen coordinates) into the destination
	static void subscreenGrab(CGDirectDisplayID display, const CGRect& screenRect, Buffer* destination)
	{
#if defined(USE_COCOA_GRAB)
		CGImageRef image = CGDisplayCreateImageForRect (display, screenRect);
		if (!image)
			throw exception(strstream() << "Grabber_OSX failed on subscreenGrab (CGDisplayCreateImageForRect)");

#else
		void* baseAddress = CGDisplayAddressForPosition(display, screenRect.origin.x, screenRect.origin.y);
		if (baseAddress == NULL)
			throw exception(strstream() << "Grabber_OSX failed on subscreenGrab (CGDisplayAddressForPosition)");
#endif

		CGColorSpaceRef bufferColorSpace = CGColorSpaceCreateDeviceRGB();
		CGContextRef bufferContext = CGBitmapContextCreateWithData(destination->data, destination->width, destination->height, 8, destination->rowLength, bufferColorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little, NULL, NULL);
		CGRect targetRect;
		targetRect.origin.x = 0;
		targetRect.origin.y = 0;
		targetRect.size.width = destination->width;
		targetRect.size.height = destination->height;

#if defined(USE_COCOA_GRAB)
		CGContextDrawImage(bufferContext, targetRect, image);
		CGImageRelease(image);
#else
		int bytesPerPixel = 4;
		int numBytesToCopy = bytesPerPixel * targetRect.size.width;
		int sourceStride = CGDisplayBytesPerRow(display);
		for (int y = 0; y < targetRect.size.height; y++) {
			uint8_t* dest = destination->data + (y * destination->rowLength);
			uint8_t* source = (uint8_t*)baseAddress + (y * sourceStride);
			memcpy(dest, source, numBytesToCopy);
		}
#endif

		CGContextRelease(bufferContext);
		CGColorSpaceRelease(bufferColorSpace);
	}

	/// Draws mouse pointer into target buffer
	/// Defined in ObjC++ File
	void drawMouseIntoBuffer(const Rect& rect, Buffer* destination);

	void GrabberOSX::grab(const Rect& rect, Buffer* destination)
	{
		DesktopTools_OSX desktopTools;

		for (uint32_t i = 0; i < desktopTools.GetScreenCount(); i++)
		{
			const Rect &displayRect(desktopTools.GetScreenResolution(i));
			Rect intersection;

			if (rect.intersects (displayRect, &intersection))
			{
				CGRect displayCoordinates;
				displayCoordinates.origin.x = intersection.x - displayRect.x;
				displayCoordinates.origin.y = intersection.y - displayRect.y;
				displayCoordinates.size.width = intersection.w;
				displayCoordinates.size.height = intersection.h;
				Buffer subBuffer;
				subBuffer.initAsSubBufferFrom(destination, intersection.x - rect.x, intersection.y - rect.y, intersection.w, intersection.h);
				
				subscreenGrab(desktopTools.GetDisplayId(i), displayCoordinates, &subBuffer);
			}
		}

		if (mEnableGrabCursor)
			drawMouseIntoBuffer(rect, destination);
	}
}

#endif
