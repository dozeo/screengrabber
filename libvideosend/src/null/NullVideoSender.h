#pragma once
#include "../VideoSender.h"

namespace dz {

class NullVideoSender : public VideoSender {
public:
	NullVideoSender ();
	virtual ~NullVideoSender();
	// Implementation
	virtual void setVideoSettings (int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality);
	virtual void setTargetFile (const std::string & filename);
	virtual void setTargetUrl (const std::string & url);
	virtual void OpenVideoStream();
	virtual void putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec);
	virtual void close ();

};

}
