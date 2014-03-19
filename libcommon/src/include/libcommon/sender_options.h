#pragma once

#include <boost/program_options.hpp>

#include "libcommon/videotypes.h"

#include <vector>

struct VideoSenderOptions
{
	VideoSenderOptions();
	friend std::ostream& operator<< (std::ostream &s, const VideoSenderOptions &o);
	
	uint32_t videowidth, videoheight; ///< Width/Height of the video
	std::string url; ///< Target URL where to send
	dz::VideoQualityLevel::Enum quality; ///< Quality level of video encoder
	dz::VideoSenderType::Enum senderType; ///< Type of sender to use
};