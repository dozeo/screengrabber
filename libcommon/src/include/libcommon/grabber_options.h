#pragma once

#include <dzlib/dzexception.h>
#include <libcommon/videotypes.h>
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
		s << "grect: " << o.m_grabRect << " gscreen: " << o.m_grabScreen << " gwid: " << o.m_grabWindowId << " gfollow: " << (o.m_grabFollow ? "true" : "false") << " gcursor: " << o.m_grabCursor << " type: " << o.m_grabberType;
		return s;
	}

	//std::string ToString() const { return dz::strstream() << "grect: " << m_grabRect << " gscreen: " << m_grabScreen << " gwid: " << m_grabWindowId << " gfollow: " << m_grabFollow << " gcursor: " << m_grabCursor << " type: " << m_grabberType; }

	bool IsGrabWindow() const { return m_grabWindowId > 0; }

	// != empty if set
	dz::Rect m_grabRect;
	
	// >= 0 if set
	int m_grabScreen; 

	// >0 if set
	int64_t m_grabWindowId;
	
	// if true, follow the grabbed region (valid on grabScreen, grabPid or grabWid)
	bool m_grabFollow;

	// Also grab mouse cursor (if possible)
	bool m_grabCursor;

	// Grabber type to use
	dz::GrabberType::Enum m_grabberType;
};