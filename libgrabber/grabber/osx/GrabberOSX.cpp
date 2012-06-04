#include "GrabberOSX.h"
#ifdef MAC_OSX
#include <assert.h>
#include <iostream>

namespace dz {

GrabberOSX::GrabberOSX () {
    mEnableGrabCursor = false;
}

GrabberOSX::~GrabberOSX () {
}

int GrabberOSX::init () {
    // nothing to do
    return 0;
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

int GrabberOSX::setEnableGrabCursor (bool enable) {
    mEnableGrabCursor = enable;
    return GE_OK;
}


/// Grabs a part of a specific display (in screen coordinates) into the destination
static int subscreenGrab (CGDirectDisplayID display, const CGRect& screenRect, Buffer * destination) {
    CGImageRef image = CGDisplayCreateImageForRect (display, screenRect);
    if (!image) {
        std::cerr << "Could not grab screen!" << std::endl;
        return GrabberOSX::GE_COULD_NOT_GRAB;
    }
    
    CGColorSpaceRef bufferColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bufferContext = CGBitmapContextCreateWithData(destination->data, destination->width, destination->height, 8, destination->rowLength, bufferColorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little, NULL, NULL);
    CGRect targetRect;
    targetRect.origin.x = 0;
    targetRect.origin.y = 0;
    targetRect.size.width  = CGImageGetWidth (image);
    targetRect.size.height = CGImageGetHeight (image); 
    
    CGContextDrawImage(bufferContext, targetRect, image);
    
    CGContextRelease (bufferContext);
    CGColorSpaceRelease (bufferColorSpace);
    CGImageRelease(image);
    return GrabberOSX::GE_OK;
}

/// Draws mouse pointer into target buffer
/// Defined in ObjC++ File
void drawMouseIntoBuffer (const Rect& rect, Buffer * destination);
    

int GrabberOSX::grab (const Rect& rect, Buffer * destination) {
    updateDisplayInformation();
    int firstError = 0;
    for (uint32_t i = 0; i < mDisplayCount; i++) {
        const Rect & displayRect (mDisplaySizes[i]);
        Rect intersection;
        if (rect.intersects (displayRect, &intersection)) {
            CGRect displayCoordinates;
            displayCoordinates.origin.x    = intersection.x - displayRect.x;
            displayCoordinates.origin.y    = intersection.y - displayRect.y;
            displayCoordinates.size.width  = intersection.w;
            displayCoordinates.size.height = intersection.h;
            Buffer subBuffer;
            
            subBuffer.initAsSubBufferFrom(destination, intersection.x - rect.x, intersection.y - rect.y, intersection.w, intersection.h);
            int code  = subscreenGrab(mDisplays[i], displayCoordinates, &subBuffer);
            firstError = firstError ? firstError : code;
        }
    }
    if (mEnableGrabCursor)
        drawMouseIntoBuffer(rect, destination);
    return firstError;
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
