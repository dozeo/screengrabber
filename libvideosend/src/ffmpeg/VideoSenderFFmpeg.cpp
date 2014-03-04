#include "VideoSenderFFmpeg.h"

#include <assert.h>

#ifdef _DEBUG
 #define FFMPEG_LOG_LEVEL (AV_LOG_VERBOSE)
#else
 #define FFMPEG_LOG_LEVEL (AV_LOG_WARNING)
#endif

namespace dz
{
	VideoSenderFFmpeg::VideoSenderFFmpeg() : _mode(OM_FILE), _frameSize(600, 400), _fps(10), _bitRate(300000), _keyframe(10), _quality(VQ_MEDIUM)
	{
		initLog();
	}

	VideoSenderFFmpeg::~VideoSenderFFmpeg()
	{
		uninitLog();
	}

	void VideoSenderFFmpeg::setVideoSettings(int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality)
	{
		_frameSize = Dimension2(w, h);
		//_frameSize.width -= (_frameSize.width % 4);
		//_frameSize.height -= (_frameSize.height % 4);

		_fps = fps;
		_bitRate = bitRate;
		_keyframe = keyframe;
		_quality = quality;
	}

	void VideoSenderFFmpeg::setTargetFile(const std::string& filename)
	{
		_url = filename;
		_mode = OM_FILE;
	}

	void VideoSenderFFmpeg::setTargetUrl(const std::string& url)
	{
		_url = url;
		_mode = OM_URL;
	}

	void VideoSenderFFmpeg::OpenVideoStream()
	{
		assert(m_videoStream.get() == nullptr);

		m_videoStream = std::unique_ptr<VideoStream>(new VideoStream(_frameSize, CODEC_ID_H264));
		if (_mode == OM_FILE)
		{
			m_videoStream->openFile(_url, _fps, _bitRate, _keyframe, _quality);
		}
		else if (_mode == OM_URL)
		{
			m_videoStream->openUrl(_url, _fps, _bitRate, _keyframe, _quality);
		}
	}

	void VideoSenderFFmpeg::putFrame(const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec)
	{
		assert(data != 0);
		assert(m_videoStream.get() != nullptr);

		Dimension2 imageSize(width, height);
		return m_videoStream->sendFrame(data, imageSize, (uint32_t)bytesPerRow, durationInSec);
	}

	void VideoSenderFFmpeg::close()
	{
		m_videoStream = nullptr;
	}

	void * gLogCallbackUser = 0;
	VideoSender::LogCallback gLogCallback = 0;

	static void avUtillogHandler (void * avcl, int level, const char* format, va_list list)
	{
#if NDEBUG
		if (level == AV_LOG_INFO)
			return;
#endif // _DEBUG

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
		::av_log_set_level(FFMPEG_LOG_LEVEL);
	}


	void VideoSenderFFmpeg::uninitLog ()
	{
		gLogCallback     = 0;
		gLogCallbackUser = 0;
		::av_log_set_level (_defaultLogLevel);
		::av_log_set_callback(&av_log_default_callback);
	}
}
