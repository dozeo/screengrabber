#include "VideoSenderFFmpeg.h"

#include <dzlib/dzexception.h>

#include <assert.h>

#ifdef _DEBUG
 #define FFMPEG_LOG_LEVEL (AV_LOG_VERBOSE)
#else
 #define FFMPEG_LOG_LEVEL (AV_LOG_WARNING)
#endif

namespace dz
{
	VideoSenderFFmpeg::VideoSenderFFmpeg(const VideoSenderOptions& options)
	{
		initLog();

		m_videoStream = std::unique_ptr<VideoStream>(new VideoStream(options.url, options.videowidth, options.videoheight, options.quality));
	}

	VideoSenderFFmpeg::~VideoSenderFFmpeg()
	{
		uninitLog();
	}

	void VideoSenderFFmpeg::putFrame(VideoFrameHandle videoFrame, double durationInSec)
	{
		return m_videoStream->SendFrame(std::move(videoFrame), durationInSec);
	}

	//virtual 
	float VideoSenderFFmpeg::GetFPS() const
	{
		if (m_videoStream == nullptr)
			throw exception("Video stream is null inside this VideoSenderFFMpeg object");

		return m_videoStream->GetFPS();
	}

	void * gLogCallbackUser = 0;
	VideoSender::LogCallback gLogCallback = 0;

	static void avUtillogHandler(void * avcl, int level, const char* format, va_list list)
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

	void VideoSenderFFmpeg::setLoggingCallback(const LogCallback& callback, void * user)
	{
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
