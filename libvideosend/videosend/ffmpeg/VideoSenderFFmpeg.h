#pragma once

#include "../VideoSender.h"
#include "VideoStream.h"
#include <grabber/Dimension.h>

namespace dz {

class VideoSenderFFmpeg : public VideoSender {
public:
	VideoSenderFFmpeg();
	virtual ~VideoSenderFFmpeg();

	virtual int setVideoSettings(int w, int h, float fps, int bitRate, enum VideoQualityLevel quality);
	virtual int setTargetFile(const std::string & filename);
	virtual int setTargetUrl(const std::string & url);
	virtual int open();
	virtual int putFrame(const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec);
	virtual void close();
	// override
	virtual void setLoggingCallback (const LogCallback& callback, void * user);

private:
	void initLog ();
	void uninitLog ();

	VideoStream* _videoStream;

	std::string _url;
	std::string _filename;

	Dimension2   _frameSize;
	float        _fps;
	int          _bitRate;
	VideoQualityLevel _quality;
	int          _defaultLogLevel;
};

}
