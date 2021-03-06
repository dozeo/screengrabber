#ifdef MAC_OSX

#include "GrabberOSX.h"

#include <assert.h>
#include <iostream>

#include <dzlib/dzexception.h>

namespace dz {

GrabberOSX::GrabberOSX () {
    mEnableGrabCursor = false;
}

GrabberOSX::~GrabberOSX () {
}

void GrabberOSX::init () {
    // nothing to do
}

void GrabberOSX::deinit () {
}

int GrabberOSX::screenCount () const {
    updateDisplayInformation ();
    return mDisplayCount;
}
    
Rect GrabberOSX::screenResolution (int screen) const {
    updateDisplayInformation ();
    if (screen >= mDisplayCount)
        return dz::Rect(); // invalid screen
    return mDisplaySizes[screen];
}
    
Rect GrabberOSX::combinedScreenResolution () const {
    updateDisplayInformation();
    if (mDisplayCount == 0) return Rect();
    if (mDisplayCount == 1) return mDisplaySizes[0];
    Rect r (mDisplaySizes[0]);
    for (int i = 1; i < mDisplayCount; i++) {
        r.addToBoundingRect(mDisplaySizes[i]);
    }
    return r;
}

void GrabberOSX::setEnableGrabCursor (bool enable) {
    mEnableGrabCursor = enable;
}
    
/// Grabs a part of a specific display (in screen coordinates) into the destination
static void subscreenGrab(CGDirectDisplayID display, const CGRect& screenRect, Buffer * destination)
{
#if defined(USE_COCOA_GRAB)
    CGImageRef image = CGDisplayCreateImageForRect (display, screenRect);
    if (!image)
        throw exception(strstream() << "Could not grab screen " << display);
#else
    void* baseAddress = CGDisplayAddressForPosition(display, screenRect.origin.x, screenRect.origin.y);
    if (baseAddress == NULL)
        throw exception(strstream() << "Could CGDisplayAddressForPosition(" << display << ")");
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
void drawMouseIntoBuffer (const Rect& rect, Buffer * destination);
    

void GrabberOSX::grab (const Rect& rect, Buffer * destination)
{
    updateDisplayInformation();

    for (uint32_t i = 0; i < mDisplayCount; i++)
    {
        const Rect & displayRect (mDisplaySizes[i]);
        Rect intersection;
        if (rect.intersects (displayRect, &intersection))
        {
            CGRect displayCoordinates;
            displayCoordinates.origin.x    = intersection.x - displayRect.x;
            displayCoordinates.origin.y    = intersection.y - displayRect.y;
            displayCoordinates.size.width  = intersection.w;
            displayCoordinates.size.height = intersection.h;
            Buffer subBuffer;
            subBuffer.initAsSubBufferFrom(destination, intersection.x - rect.x, intersection.y - rect.y, intersection.w, intersection.h);
            
            subscreenGrab(mDisplays[i], displayCoordinates, &subBuffer);
        }
    }
    
    if (mEnableGrabCursor)
        drawMouseIntoBuffer(rect, destination);
}
    
void GrabberOSX::updateDisplayInformation () const {
    uint32_t max = sizeof (mDisplays) / sizeof (CGDirectDisplayID);
    /*CGGetOnlineDisplayList would also return sleeping and mirrored displays*/ 
    CGGetActiveDisplayList(max, mDisplays, &mDisplayCount);
    for (uint32_t i = 0; i < mDisplayCount; i++) {
        CGRect bounds = CGDisplayBounds (mDisplays[i]);
        mDisplaySizes[i] = Rect (bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
    }
}

    
}
#endif
