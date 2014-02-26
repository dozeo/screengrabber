#ifdef MAC_OSX

#include "DesktopTools_OSX.h"

namespace dz
{
	DesktopTools_OSX::DesktopTools_OSX()
	{
		const uint32_t max = sizeof(mDisplays) / sizeof (CGDirectDisplayID);

		/*CGGetOnlineDisplayList would also return sleeping and mirrored displays*/ 
		CGGetActiveDisplayList(max, mDisplays, &mDisplayCount);
		for (uint32_t i = 0; i < mDisplayCount; i++)
		{
			CGRect bounds = CGDisplayBounds(mDisplays[i]);
			mDisplaySizes[i] = Rect(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
		}
	}

	DesktopTools_OSX::~DesktopTools_OSX()
	{
	}

	uint32_t DesktopTools_OSX::GetScreenCount() const
	{
		return mDisplayCount;
	}

	Rect IDesktopTools::GetScreenResolution(uint32_t screen) const
	{
		if (screen >= mDisplayCount)
			return dz::Rect(); // invalid screen
		return mDisplaySizes[screen];
	}

	Rect IDesktopTools::GetCombinedScreenResolution() const
	{
		if (mDisplayCount == 0)
			return Rect();
		
		if (mDisplayCount == 1)
			return mDisplaySizes[0];

		Rect r (mDisplaySizes[0]);
		for (int i = 1; i < mDisplayCount; i++)
			r.addToBoundingRect(mDisplaySizes[i]);
		
		return r;
	}
}


#endif // MAC_OSX
