#pragma once
#include <videosend/VideoSender.h>
#include <boost/program_options.hpp>
#include <ostream>
#include <vector>

/// Options for the video sender
struct VideoSenderOptions {
	VideoSenderOptions () :
		fps (10.0f),
		kiloBitrate (100),
		keyframe (10),
		width (640),
		height(480),
		cutSize (true),
		correctAspect (true),
		quality (dz::VQ_MEDIUM),
		type (dz::VT_DEFAULT){}

	std::ostream & operator<< (std::ostream & s) const {
		s << "fps: " << fps << " kiloBitrate: " << kiloBitrate << " keyframe: " << keyframe << " vsize: " << width << "x" << height;
		if (!url.empty()) {
			s << " url: " << url;
		} else {
			s << " file: " << file;
		}
		s << " type: " << type;
		s << " quality: " << quality;
		return s;
	}

	inline std::vector<std::string> packCommandLine () const;


	float fps; ///< fps rate to send
	int kiloBitrate; ///< bitrate in kiloBit
	int keyframe; ///< keyframe, the number of frames the next keyframe is sent
	int width, height; ///< Width/Height of the video
	bool cutSize;  ///< Cut down the video size so that it matches the source
	bool correctAspect; ///< Correct the aspect of width/height
	std::string url; ///< Target URL where to send
	std::string file; ///< Target file where to save (if URL is not set)
	dz::VideoQualityLevel quality; ///< Quality level of video encoder
	dz::VideoSenderType type; ///< Type of sender to use
};

inline std::ostream & operator<< (std::ostream & s, const VideoSenderOptions & o) { return o.operator<<(s); }

inline std::vector<std::string> VideoSenderOptions::packCommandLine () const {
	std::vector<std::string> result;
	if (type != dz::VT_DEFAULT) {
		const char * nameType = "invalid";
		switch (type) {
		case dz::VT_NULL:
			nameType = "Null";
			break;
		case dz::VT_DEFAULT:
			nameType = "Default";
			break;
		case dz::VT_QT:
			nameType = "Qt";
			break;
		default:
			assert (!"should not come here, a new type?");
			break;
		}
		result.push_back ("--vtype");
		result.push_back (nameType);
	}
	
	const char * qualityName = "invalid";
	switch (quality) {
	case dz::VQ_LOW:
		qualityName = "Low";
		break;
	case dz::VQ_MEDIUM:
		qualityName = "Medium";
		break;
	case dz::VQ_HIGH:
		qualityName = "High";
		break;
	default:
		assert (!"should not come here, a new quality level?");
		break;
	}
	result.push_back ("--quality");
	result.push_back (qualityName);

	result.push_back ("--vsize");
	result.push_back (boost::lexical_cast<std::string> (width) + "," + boost::lexical_cast<std::string> (height));
	result.push_back ("--bitrate");
	result.push_back (boost::lexical_cast<std::string> (kiloBitrate));
	result.push_back ("--keyframe");
	result.push_back (boost::lexical_cast<std::string> (keyframe));
	result.push_back ("--fps");
	result.push_back (boost::lexical_cast<std::string> (fps));
	if (!file.empty()){
		result.push_back ("--file");
		result.push_back (file);
	}
	if (!url.empty()){
		result.push_back ("--url");
		result.push_back (url);
	}
	result.push_back ("--correctAspect");
	result.push_back (correctAspect ? "true" : "false");
	result.push_back ("--cutSize");
	result.push_back (cutSize ? "true" : "false");
	return result;
}

/// Boost program options parser for VideoSenderOptions
struct VideoSenderOptionsParser {
	VideoSenderOptionsParser (VideoSenderOptions * _target) : target (_target), desc ("Video Sending Options") {
		desc.add_options()
				("vsize", boost::program_options::value<std::string>(), "Video Resolution w,h")
				("cutSize", boost::program_options::value<bool> (&target->cutSize)->default_value(true), "Cut the video size to the source")
				("correctAspect", boost::program_options::value<bool> (&target->correctAspect)->default_value(true), "Correct Aspect")
				("vtype", boost::program_options::value<std::string>(), "Sender Type (Null|Default|Qt)")
				("quality", boost::program_options::value<std::string>(), "Quality Level (Low|Medium|High)")
				("fps", boost::program_options::value<float>(&target->fps)->default_value (10.0f), "Frames per Second")
				("bitrate", boost::program_options::value<int>(&target->kiloBitrate)->default_value(100), "Bitrate in kbit/s")
				("keyframe", boost::program_options::value<int>(&target->keyframe)->default_value(10), "Keyframe, number of frames the next keyframe is sent")
				("url", boost::program_options::value<std::string>(&target->url),
					"URL where to send, e.g.\n"
					"  RTMP: rtmp://[host]/[app]/[playpath]"
					"        rtmp://[host]/[app] playpath=test"
					"        rtmp://[host] app=appname/1234 playpath=1234/1234"
					"  TCP: tcp://[host]:[port]/[app]/[playpath]")
				("file", boost::program_options::value<std::string>(&target->file)->default_value ("screencast.flv"), "File where to write out (when no URL is set)");
	}

	void apply (const boost::program_options::variables_map & po) {
		if (po.count ("vsize") > 0) {
			std::string arg = po["vsize"].as<std::string>();
			int count = sscanf (arg.c_str(), "%d,%d", &target->width, &target->height);
			if (count != 2 || target->width < 1 || target->height < 1) {
				throw boost::program_options::invalid_option_value("Invalid vsize");
			}
		}
		if (po.count ("vtype") > 0) {
			std::string arg = po["vtype"].as<std::string>();
			if (arg == "Null") target->type = dz::VT_NULL;
			else if (arg == "Default") target->type = dz::VT_DEFAULT;
			else if (arg == "Qt") target->type = dz::VT_QT;
			else throw boost::program_options::invalid_option_value("Invalid vtype " + arg);
		}
		if (po.count ("quality") > 0) {
			std::string arg = po["quality"].as<std::string>();
			if (arg == "Low") target->quality = dz::VQ_LOW;
			else if (arg == "Medium") target->quality = dz::VQ_MEDIUM;
			else if (arg == "High") target->quality = dz::VQ_HIGH;
			else throw boost::program_options::invalid_option_value("Invalid quality " + arg);
		}
		if (target->fps < 0.5) {
			throw boost::program_options::invalid_option_value ("Invalid fps");
		}
		if (target->kiloBitrate < 10) {
			throw boost::program_options::invalid_option_value ("Invalid bitrate");
		}
		if (target->keyframe < 2 || target->keyframe > 100) {
			throw boost::program_options::invalid_option_value ("Invalid keyframe, must be between 2 and 100");
		}
	}

	VideoSenderOptions * target;
	boost::program_options::options_description desc; ///< Boost program_options description
};

