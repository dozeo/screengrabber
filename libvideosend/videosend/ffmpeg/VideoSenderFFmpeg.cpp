#include "VideoSenderFFmpeg.h"

#include <assert.h>

namespace dz {

VideoSenderFFmpeg::VideoSenderFFmpeg()
: _frameSize(Dimension2(600, 400))
, _fps(10)
, _bitRate(300000)
, _videoStream(0)
, _quality(VQ_MEDIUM)
{
	initLog ();
}

VideoSenderFFmpeg::~VideoSenderFFmpeg() {
	close();
	uninitLog();
}

int VideoSenderFFmpeg::setVideoSettings(int w, int h, float fps, int bitRate, enum VideoQualityLevel quality) {
	assert(!_videoStream && "already started?");
	_frameSize = Dimension2(w, h);
	_fps = fps;
	_bitRate = bitRate;
	_quality = quality;
	return VE_OK;
}

int VideoSenderFFmpeg::setTargetFile(const std::string& filename) {
	assert(!_videoStream && "already started?");
	if (!_url.empty()) {
		return VE_INVALID_TARGET;
	}
	_filename = filename;
	return VE_OK;
}

int VideoSenderFFmpeg::setTargetUrl(const std::string& url)
{
	assert(!_videoStream && "already started?");
	if (!_filename.empty()) {
		return VE_INVALID_TARGET;
	}
	_url = url;
	return VE_OK;
}

int VideoSenderFFmpeg::open() {
	_videoStream = new VideoStream(_frameSize, CODEC_ID_H264);
	if (!_filename.empty()) {
		return _videoStream->openFile(_filename, _fps, _bitRate, _quality);
	}
	else if (!_url.empty()) {
		return _videoStream->openUrl(_url, _fps, _bitRate, _quality);
	}
	return VE_OK;
}

int VideoSenderFFmpeg::putFrame(const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec)
{
	assert(data != 0);
	assert(_videoStream != 0 && "video stream was not opened");

	Dimension2 imageSize(width, height);
	return _videoStream->sendFrame(data, imageSize, (uint32_t)bytesPerRow, durationInSec);
}

void VideoSenderFFmpeg::close()
{	
	if (_videoStream != 0) {
		_videoStream->close();
		delete _videoStream;
		_videoStream = 0;
	}
}

void * gLogCallbackUser = 0;
VideoSender::LogCallback gLogCallback = 0;

static void avUtillogHandler (void * avcl, int level, const char* format, va_list list) {
	char buffer[1024];
	buffer[sizeof(buffer)-1] = 0;

	static int printPrefix = 1;
	av_log_format_line (avcl, level, format, list, buffer, sizeof(buffer), &printPrefix);
	if (gLogCallback) {
		gLogCallback (std::string (buffer), gLogCallbackUser);
	}
}

void VideoSenderFFmpeg::setLoggingCallback (const LogCallback& callback, void * user) {
	gLogCallbackUser = user;
	gLogCallback = callback;
	::av_log_set_callback(&avUtillogHandler);
}


void VideoSenderFFmpeg::initLog ()
{
	_defaultLogLevel = ::av_log_get_level ();
	::av_log_set_level (AV_LOG_VERBOSE);
}


void VideoSenderFFmpeg::uninitLog ()
{
	gLogCallback     = 0;
	gLogCallbackUser = 0;
	::av_log_set_level (_defaultLogLevel);
	::av_log_set_callback(&av_log_default_callback);
}


}
