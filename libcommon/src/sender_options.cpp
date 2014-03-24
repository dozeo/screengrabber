#include "libcommon/sender_options.h"

VideoSenderOptions::VideoSenderOptions() : videowidth(0), videoheight(0), quality(dz::VideoQualityLevel::Medium), senderType(dz::VideoSenderType::Default)
{
}

std::ostream& operator<< (std::ostream &s, const VideoSenderOptions &o)
{
	s << " vsize: " << o.videowidth << "x" << o.videoheight;
	s << " url: " << o.url;
	s << " type: " << o.senderType;
	s << " quality: " << o.quality;
	return s;
}