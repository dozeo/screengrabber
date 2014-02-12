#ifdef MAC_OSX

#include "../WindowInfo.h"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

namespace dz {
    
static int64_t toWid (CFNumberRef r) {
    CGWindowID v;
    CFNumberGetValue (r, kCGWindowIDCFNumberType, &v);
    return v;
}
    
static int64_t toPid (CFNumberRef r) {
    int v;
    CFNumberGetValue (r, kCFNumberIntType, &v);
    return v;
}

    
static std::string toStdStringIfSet (CFStringRef s) {
    if (!s) return std::string();
    char buffer[1024] = "";
    CFStringGetCString(s, buffer, sizeof buffer -1, kCFStringEncodingUTF8);
    return buffer;
}

dz::Rect toRect (CFDictionaryRef dict) {
    CGRect r;
    CGRectMakeWithDictionaryRepresentation(dict, &r);
    return dz::Rect ((int) r.origin.x, (int) r.origin.y, (int) r.size.width, (int) r.size.height);
}
    
static WindowInfo toWindowInfo (CFDictionaryRef d) {
    CFNumberRef widRef = (CFNumberRef) CFDictionaryGetValue(d, kCGWindowNumber);
    CFNumberRef pidRef = (CFNumberRef) CFDictionaryGetValue(d, kCGWindowOwnerPID);
    CFDictionaryRef boundsRef = (CFDictionaryRef) CFDictionaryGetValue (d, kCGWindowBounds);
    
    // optional
    CFStringRef windowNameRef = (CFStringRef) CFDictionaryGetValue (d, kCGWindowName);
    CFStringRef ownerNameRef  = (CFStringRef) CFDictionaryGetValue (d, kCGWindowOwnerName);
    
    WindowInfo info;
    info.pid  = toPid (pidRef);
    info.id   = toWid (widRef);
    info.area = toRect (boundsRef);
    info.title = toStdStringIfSet(windowNameRef);
    if (info.title.empty()) 
        info.title = toStdStringIfSet(ownerNameRef);
    // info.title can still be empty, but we don't care.
    return info;
}
    
void WindowInfo::populate(std::vector<WindowInfo>* destination, int64_t pidFilter)
{
	CFArrayRef windows = CGWindowListCopyWindowInfo (kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
	int count = CFArrayGetCount (windows);

	for (CFIndex i = 0; i < count; i++) 
	{
		// should be in there
		CFDictionaryRef d = (CFDictionaryRef) CFArrayGetValueAtIndex (windows, i);
		if (pidFilter != 0)
		{
			CFNumberRef pidRef = (CFNumberRef) CFDictionaryGetValue(d, kCGWindowOwnerPID);
			if (toPid (pidRef) != pidFilter)
				continue; // not interested in
		}
		WindowInfo info = toWindowInfo (d);
		destination->push_back (info);
	}

	CFRelease (windows);
}

/*static*/ WindowInfo WindowInfo::about (int64_t wid) {
    CGWindowID windowIds[1] = { static_cast<CGWindowID>(wid) };
    CFArrayRef widArray = CFArrayCreate (NULL, (const void**) windowIds, 1, NULL);
    CFArrayRef windows = CGWindowListCreateDescriptionFromArray(widArray);
    
    WindowInfo result;
    if (CFArrayGetCount(windows) == 1) {
        CFDictionaryRef d = (CFDictionaryRef) CFArrayGetValueAtIndex(windows, 0);
        CFBooleanRef b = (CFBooleanRef) CFDictionaryGetValue(d, kCGWindowIsOnscreen);
        if (b && CFBooleanGetValue(b)){
            result = toWindowInfo (d);
        }
    }
    CFRelease (windows);
    CFRelease (widArray);
    return result;
}

    
    
}

#endif
