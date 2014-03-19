#pragma once

#include <libcommon/sender_options.h>
#include <libvideosend/src/VideoSender.h>
#include <boost/program_options.hpp>
#include <ostream>
#include <vector>

// packs video sender options for the command line
class VideoSenderOptionsCmdLine
{
	public:
		static std::vector<std::string> packCommandLine(VideoSenderOptions& target)
		{
			std::vector<std::string> result;

			if (target.senderType != dz::VideoSenderType::Default)
			{
				result.push_back("--vtype");
				result.push_back(dz::VideoSenderType::ToString(target.senderType));
			}
	
			result.push_back("--quality");
			result.push_back( dz::VideoQualityLevel::ToString(target.quality) );

			result.push_back("--vsize");
			result.push_back(boost::lexical_cast<std::string>(target.videowidth) + "," + boost::lexical_cast<std::string>(target.videoheight));

			result.push_back("--url");
			result.push_back(target.url);

			return result;
		}
};

