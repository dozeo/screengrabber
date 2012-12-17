#pragma once
#include "../VideoSender.h"

namespace dz {

class NullVideoSender : public VideoSender {
public:
	NullVideoSender ();
	virtual ~NullVideoSender();
	// Implementation
	virtual int setVideoSettings (int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality);
	virtual int setTargetFile (const std::string & filename);
	virtual int setTargetUrl (const std::string & url);
	virtual int open ();
	virtual int putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec);
	virtual void close ();

};

}
