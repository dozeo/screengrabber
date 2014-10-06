#ifdef _WIN32

#include "DesktopTools_Win32.h"
#include <dzlib/dzexception.h>
#include <slog/slog.h>

namespace dz
{
	DesktopTools_Win32::DesktopTools_Win32()
	{
		ScreenEnumerator enumerator;
		enumerator.enumerate();
		m_displays = enumerator.displays();
		if (m_displays.size() == 0)
			throw exception(strobj() << "DesktopTools_Win32 - found no displays");
	}
	
	DesktopTools_Win32::~DesktopTools_Win32()
	{
	}

	uint32_t DesktopTools_Win32::GetScreenCount() const
	{
		return m_displays.size();
	}

	Rect DesktopTools_Win32::GetScreenResolution(uint32_t screen) const
	{
		if (screen > m_displays.size())
			throw exception(strobj() << "DesktopTools_Win32::GetScreenResolution(" << screen << ") - Invalid display number given. Only " << m_displays.size() << " display(s) found");

		return m_displays[screen].rect;
	}

	Rect DesktopTools_Win32::GetCombinedScreenResolution() const
	{
		uint32_t screenCount = GetScreenCount();

		Rect r;
		for (uint32_t i = 0; i < screenCount; i++)
			r.addToBoundingRect(GetScreenResolution(i));
		
		return r;
	}

	bool DesktopTools_Win32::IsInsideAnyScreen(const Rect& otherRect) const
	{
		for (uint32_t i = 0; i < GetScreenCount(); i++)
		{
			Rect screenRect = GetScreenResolution(i);
			if (screenRect.contains(otherRect.x, otherRect.y) &&
				screenRect.contains(otherRect.x + otherRect.width, otherRect.y + otherRect.height))
				return true;
		}

		return false;
	}
}

#endif // _WIN32
