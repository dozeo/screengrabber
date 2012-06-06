#include <Cocoa/Cocoa.h>

extern "C" {
    // Initialize NSApplication
    // This is necessary to use 
    // Some calls to the window manager (e.g. Current Cursor)
    void initalizeNSApplication ();
}


void initalizeNSApplication () {
    [NSApplication sharedApplication];
}
