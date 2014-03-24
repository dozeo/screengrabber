#pragma once

#include "VideoSender.h"
#include "VideoStream.h"

#include <libgrabber/src/Dimension.h>
#include <libcommon/videoframe.h>

#include <memory>

namespace dz
{
	class VideoSenderFFmpeg : public VideoSender
	{
		public:
			VideoSenderFFmpeg(const VideoSenderOptions& options);
			virtual ~VideoSenderFFmpeg();

			virtual void SendFrame(VideoFrameHandle videoFrame, double durationInSec);
			virtual float GetFPS() const;

			// override
			virtual void setLoggingCallback (const LogCallback& callback, void * user);
			virtual const Statistic* statistic() const { return m_videoStream ? m_videoStream->statistic() : 0; }

		private:
			void initLog();
			void uninitLog();

			std::unique_ptr<VideoStream> m_videoStream;

			int _defaultLogLevel;

			VideoSenderOptions m_options;
			uint32_t m_lastSendFrameWidth, m_lastSendFrameHeight;
			uint32_t m_stableFrameSizeCount;
	};
}
