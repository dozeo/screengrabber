#include "../igrabber.h"

#include <Cocoa/Cocoa.h>
#include  <iostream>
namespace dz {

// Declared in GrabberOSX.cpp
void drawMouseIntoBuffer(const Rect& rect, VideoFrameHandle& frame) {
    // Note: 
    // - Mouse position is measured from lower left = 0,0
    // - Hotspot in image is measured from upper left
    // - Drawing via image drawAtPoint is measured from lower left (drawing image upside from given point)
    // - NSCursor needs NSApplication initialized
    // Real Coordinates: Coordinate space with 0,0 upper left and y+1 = down
    // OSX  Coordinates: Coordinate space with 0,0 lower left and y+1 = up
    
    int mainDisplayHeight = (int) (CGDisplayBounds (kCGDirectMainDisplay).size.height);
    
    NSPoint p = [NSEvent mouseLocation];
    NSCursor * currentCursor = [NSCursor currentSystemCursor];
    if (currentCursor == 0) {
        // could not figure out current cursor
        // Do you initialized NSApplication?
        return;
    }
    NSPoint hotSpot = [currentCursor hotSpot];
    NSImage *  imageFromCursor = [currentCursor image];
    NSSize s = [imageFromCursor size];
    
    // Bounding Box in "real coordinates"
    dz::Rect boundingBox = dz::Rect (p.x - hotSpot.x, (mainDisplayHeight - p.y) - hotSpot.y, s.width, s.height);
    if (!rect.intersects(boundingBox)) {
        // invisible
        return;
    }
    
    // In OSX Coordinates
    NSPoint whereToDraw = NSMakePoint (
                                       p.x - hotSpot.x - rect.x, 
                                       p.y - (s.height - hotSpot.y) - (mainDisplayHeight - rect.y - rect.height)
                                       );

    
    CGColorSpaceRef bufferColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bufferContext = CGBitmapContextCreateWithData(frame->GetData(), frame->GetWidth(), frame->GetHeight(), 8, frame->GetStride(), bufferColorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little, NULL, NULL);

    NSGraphicsContext * ctxt = [NSGraphicsContext graphicsContextWithGraphicsPort: bufferContext flipped: FALSE];
    [NSGraphicsContext saveGraphicsState];
    
    [NSGraphicsContext setCurrentContext:ctxt];
    
    
    [imageFromCursor drawAtPoint:whereToDraw fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
    
    
    [NSGraphicsContext restoreGraphicsState];
    [ctxt release];
    
    CGContextRelease (bufferContext);
    CGColorSpaceRelease (bufferColorSpace);
}

    
}
