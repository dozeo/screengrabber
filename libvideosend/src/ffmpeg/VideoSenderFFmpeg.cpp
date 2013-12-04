#include "VideoSenderFFmpeg.h"

#include <assert.h>

namespace dz
{
	VideoSenderFFmpeg::VideoSenderFFmpeg()
	: _frameSize(600, 400)
	, _fps(10)
	, _bitRate(300000)
	, _keyframe(10)
	, _videoStream(0)
	, _quality(VQ_MEDIUM)
	, _mode(OM_FILE)
	{
		initLog();
	}

	VideoSenderFFmpeg::~VideoSenderFFmpeg()
	{
		close();
		uninitLog();
	}

	int VideoSenderFFmpeg::setVideoSettings(int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality)
	{
		_frameSize = Dimension2(w, h);
		_fps = fps;
		_bitRate = bitRate;
		_keyframe = keyframe;
		_quality = quality;
		return 0;
	}

	int VideoSenderFFmpeg::setTargetFile(const std::string& filename)
	{
		_url = filename;
		_mode = OM_FILE;
		return 0;
	}

	int VideoSenderFFmpeg::setTargetUrl(const std::string& url)
	{
		_url = url;
		_mode = OM_URL;
		return 0;
	}

	int VideoSenderFFmpeg::open()
	{
		assert(_videoStream == 0);

		int result = 0;
		_videoStream = new VideoStream(_frameSize, CODEC_ID_H264);
		if (_mode == OM_FILE)
		{
			result = _videoStream->openFile(_url, _fps, _bitRate, _keyframe, _quality);
		}
		else if (_mode == OM_URL)
		{
			result = _videoStream->openUrl(_url, _fps, _bitRate, _keyframe, _quality);
		}

		return result;
	}

	int VideoSenderFFmpeg::putFrame(const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec)
	{
		assert(data != 0);
		assert(_videoStream != 0);

		Dimension2 imageSize(width, height);
		return _videoStream->sendFrame(data, imageSize, (uint32_t)bytesPerRow, durationInSec);
	}

	void VideoSenderFFmpeg::close()
	{	
		if (_videoStream != 0)
		{
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

	void VideoSenderFFmpeg::initLog()
	{
		_defaultLogLevel = ::av_log_get_level ();
		::av_log_set_level(AV_LOG_VERBOSE);
	}


	void VideoSenderFFmpeg::uninitLog ()
	{
		gLogCallback     = 0;
		gLogCallbackUser = 0;
		::av_log_set_level (_defaultLogLevel);
		::av_log_set_callback(&av_log_default_callback);
	}
}
