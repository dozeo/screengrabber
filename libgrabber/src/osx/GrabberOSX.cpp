#ifdef MAC_OSX

#include "GrabberOSX.h"
#include "../DesktopTools_OSX.h"
#include "../grabber_null.h"

#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

#include <libcommon/videoframe.h>
#include <libcommon/videoframepool.h>

#define BITS_PER_PIXEL 32

namespace dz
{
	GrabberOSX::GrabberOSX ()
	{
		mEnableGrabCursor = false;
	}

	GrabberOSX::~GrabberOSX() {}

	IGrabber* IGrabber::CreateGrabber(const GrabberOptions& options)
	{
		IGrabber* grabber = nullptr;

		int64_t windowId = options.m_grabWindowId;
		if (windowId != -1)
			//grabber = new WindowGrabber_OSX(windowId);
			grabber = new NullGrabber();
		else if (options.m_grabberType == GrabberType::Null)
			grabber = new NullGrabber();
		else
		{
			Rect captureRect = options.m_grabRect;
			grabber = new GrabberOSX();
			if (grabber)
				grabber->SetCaptureRect(captureRect);
		}

		grabber->setEnableGrabCursor(options.m_grabCursor);

		return grabber;
	}

	void GrabberOSX::SetCaptureRect(Rect capture)
	{
		_capture_rect = capture;
	}

	void GrabberOSX::setEnableGrabCursor(bool enable)
	{
		mEnableGrabCursor = enable;
	}

// 	/// Grabs a part of a specific display (in screen coordinates) into the destination
// 	static void subscreenGrab(CGDirectDisplayID display, const CGRect& screenRect, Buffer* destination)
// 	{
// #if defined(USE_COCOA_GRAB)
// 		CGImageRef image = CGDisplayCreateImageForRect(display, screenRect);
// 		if (!image)
// 			throw exception(strstream() << "GrabberOSX failed on subscreenGrab (CGDisplayCreateImageForRect)");

// #else
// 		void* baseAddress = CGDisplayAddressForPosition(display, screenRect.origin.x, screenRect.origin.y);
// 		if (baseAddress == NULL)
// 			throw exception(strstream() << "GrabberOSX failed on subscreenGrab (CGDisplayAddressForPosition)");
// #endif

// 		CGColorSpaceRef bufferColorSpace = CGColorSpaceCreateDeviceRGB();
// 		CGContextRef bufferContext = CGBitmapContextCreateWithData(destination->data, destination->width, destination->height, 8, destination->rowLength, bufferColorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little, NULL, NULL);
// 		CGRect targetRect;
// 		targetRect.origin.x = 0;
// 		targetRect.origin.y = 0;
// 		targetRect.size.width = destination->width;
// 		targetRect.size.height = destination->height;

// #if defined(USE_COCOA_GRAB)
// 		CGContextDrawImage(bufferContext, targetRect, image);
// 		CGImageRelease(image);
// #else
// 		int bytesPerPixel = 4;
// 		int numBytesToCopy = bytesPerPixel * targetRect.size.width;
// 		int sourceStride = CGDisplayBytesPerRow(display);
// 		for (int y = 0; y < targetRect.size.height; y++) {
// 			uint8_t* dest = destination->data + (y * destination->rowLength);
// 			uint8_t* source = (uint8_t*)baseAddress + (y * sourceStride);
// 			memcpy(dest, source, numBytesToCopy);
// 		}
// #endif

// 		CGContextRelease(bufferContext);
// 		CGColorSpaceRelease(bufferColorSpace);
// 	}

	/// Draws mouse pointer into target buffer
	/// Defined in ObjC++ File
	void drawMouseIntoBuffer(const Rect& rect, VideoFrameHandle& destination);

	VideoFrameHandle GrabberOSX::GrabVideoFrame()
	{
		VideoFrameFormat::Enum format = (BITS_PER_PIXEL == 32) ? VideoFrameFormat::RGBA : VideoFrameFormat::RGB;
		auto frame(VideoFramePool::GetInstance().AllocVideoFrame(_capture_rect.width, _capture_rect.height, format));
		if (frame)
		{
			drawMouseIntoBuffer(_capture_rect, frame);
		}

		return std::move(frame);
	}

	// void GrabberOSX::grab(const Rect& rect, Buffer* destination)
	// {
	// 	DesktopTools_OSX desktopTools;

	// 	for (uint32_t i = 0; i < desktopTools.GetScreenCount(); i++)
	// 	{
	// 		const Rect &displayRect(desktopTools.GetScreenResolution(i));
	// 		Rect intersection;

	// 		if (rect.intersects(displayRect, &intersection))
	// 		{
	// 			CGRect displayCoordinates;
	// 			displayCoordinates.origin.x = intersection.x - displayRect.x;
	// 			displayCoordinates.origin.y = intersection.y - displayRect.y;
	// 			displayCoordinates.size.width = intersection.width;
	// 			displayCoordinates.size.height = intersection.height;
	// 			Buffer subBuffer;
	// 			subBuffer.initAsSubBufferFrom(destination, intersection.x - rect.x, intersection.y - rect.y, intersection.width, intersection.height);
				
	// 			subscreenGrab(desktopTools.GetDisplayId(i), displayCoordinates, &subBuffer);
	// 		}
	// 	}

	// 	if (mEnableGrabCursor)
	// 		drawMouseIntoBuffer(rect, destination);
	// }
}

#endif
