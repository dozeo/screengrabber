#ifdef MAC_OSX

#include "DesktopTools_OSX.h"

namespace dz
{
	DesktopTools_OSX::DesktopTools_OSX()
	{
		CGDirectDisplayID displays[16];

		const uint32_t max = sizeof(displays) / sizeof (CGDirectDisplayID);

		/*CGGetOnlineDisplayList would also return sleeping and mirrored displays*/ 
		CGGetActiveDisplayList(max, displays, &m_displayCount);
		for (uint32_t i = 0; i < m_displayCount; i++)
		{
			CGRect bounds = CGDisplayBounds(displays[i]);
			m_displaySizes[i] = Rect(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
		}
	}

	DesktopTools_OSX::~DesktopTools_OSX()
	{
	}

	uint32_t DesktopTools_OSX::GetScreenCount() const
	{
		return m_displayCount;
	}

	Rect DesktopTools_OSX::GetScreenResolution(uint32_t screen) const
	{
		if (screen >= m_displayCount)
			throw exception(strstream() << "DesktopTools_OSX::GetScreenResolution() - Invalid screenID " << screen << " given. The maximum is " << m_displayCount);

		return m_displaySizes[screen];
	}

	Rect DesktopTools_OSX::GetCombinedScreenResolution() const
	{
		if (m_displayCount == 0)
			return Rect();
		
		if (m_displayCount == 1)
			return m_displaySizes[0];

		Rect r(m_displaySizes[0]);
		for (uint32_t i = 1; i < m_displayCount; i++)
			r.addToBoundingRect(m_displaySizes[i]);
		
		return r;
	}
}


#endif // MAC_OSX
