#pragma once

#include <dzlib/dzexception.h>
#include <libcommon/grabber_type.h>
#include <libcommon/dzrect.h>

#include <cstdint>
#include <string>
#include <sstream>

/// Options for the screen grabber
struct GrabberOptions
{
	GrabberOptions() : m_grabScreen(-1), m_grabWindowId(-1), m_grabFollow(true), m_grabCursor(false), m_grabberType(dz::GrabberType::Default) {}

	friend std::ostream& operator<< (std::ostream& s, const GrabberOptions& o)
	{
		s << o.ToString();
		return s;
	}

	std::string ToString() const { return dz::strstream() << "grect: " << m_grabRect << " gscreen: " << m_grabScreen << " gwid: " << m_grabWindowId << " gfollow: " << m_grabFollow << " gcursor: " << m_grabCursor << " type: " << m_grabberType; }

	bool IsGrabWindow() const { return m_grabWindowId > 0; }

	dz::Rect m_grabRect;  ///< != empty if set
	int m_grabScreen;     ///< >= 0 if set
	int64_t m_grabWindowId;    ///<   >0 if set
	bool m_grabFollow;    ///<   if true, follow the grabbed region (valid on grabScreen, grabPid or grabWid)
	bool m_grabCursor;    ///<   Also grab mouse cursor (if possible)
	dz::GrabberType::Enum m_grabberType; ///< Grabber type to use
};