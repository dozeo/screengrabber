#include "VideoSenderFFmpeg.h"

#include <dzlib/dzexception.h>

#include <assert.h>

#include <boost/thread/thread.hpp>

#ifdef _DEBUG
 #define FFMPEG_LOG_LEVEL (AV_LOG_VERBOSE)
#else
 #define FFMPEG_LOG_LEVEL (AV_LOG_WARNING)
#endif

namespace dz
{
	VideoSenderFFmpeg::VideoSenderFFmpeg(const VideoSenderOptions& options) : m_options(options), m_lastSendFrameWidth(0), m_lastSendFrameHeight(0), m_stableFrameSizeCount(0)
	{
		initLog();

		m_videoStream = std::unique_ptr<VideoStream>(new VideoStream(m_options.url, m_options.videowidth, m_options.videoheight, m_options.quality));
		m_lastSendFrameWidth = m_options.videowidth;
		m_lastSendFrameHeight = m_options.videoheight;
	}

	VideoSenderFFmpeg::~VideoSenderFFmpeg()
	{
		uninitLog();
	}

	void VideoSenderFFmpeg::SendFrame(VideoFrameHandle videoFrame, double durationInSec)
	{
		if (videoFrame->GetWidth() != m_lastSendFrameWidth ||
			videoFrame->GetHeight() != m_lastSendFrameHeight)
		{
			m_stableFrameSizeCount = 0;
			m_lastSendFrameWidth = videoFrame->GetWidth();
			m_lastSendFrameHeight = videoFrame->GetHeight();
		}
		else
			m_stableFrameSizeCount++;

		// half a second of frame size stability before we re establish the stream
		const uint32_t reqFrames = static_cast<uint32_t>(m_videoStream->GetFPS()) / 2;

		if (m_stableFrameSizeCount >= reqFrames)
		{
			const bool bNeedsResize = (videoFrame->GetWidth() != m_options.videowidth) || (videoFrame->GetHeight() != m_options.videoheight);
			if (bNeedsResize)
			{
				m_videoStream = nullptr;
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

				m_options.videowidth = videoFrame->GetWidth();
				m_options.videoheight = videoFrame->GetHeight();
				m_videoStream = std::unique_ptr<VideoStream>(new VideoStream(m_options.url, m_options.videowidth, m_options.videoheight, m_options.quality));
			}
		}

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
