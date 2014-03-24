#pragma once

#include <libgrabber/src/Dimension.h>
#include "VideoSender.h"
#include "ffmpeg_includes.h"
#include "FFmpegUtils.h"

#include <iostream>
#include <memory>

#include <libcommon/videotypes.h>

namespace dz
{
	typedef UniquePtrCustom<AVStream> SmartPtrAVStream;
	typedef UniquePtrCustom<SwsContext> SmartPtrSwsContext;
	typedef UniquePtrCustom<uint8_t> SmartPtrAvMalloc;

	class VideoStream
	{
		public:
			VideoStream(const std::string& url, uint32_t width, uint32_t height, VideoQualityLevel::Enum level = VideoQualityLevel::Medium);
			virtual ~VideoStream();

			void SendFrame(VideoFrameHandle videoFrame, double timeDurationInSeconds);

			float GetFPS() const;
			uint32_t GetVideoWidth() const { return m_videoFrameWidth; }
			uint32_t GetVideoHeight() const { return m_videoFrameHeight; }

			const VideoSender::Statistic* statistic() const { return &_statistic; }

		private:
			SmartPtrAVStream addVideoStream(VideoQualityLevel::Enum level);

			void SetBasicSettings(AVCodecContext* codec, VideoQualityLevel::Enum level);

			void setupScaleContext(uint32_t srcWidth, uint32_t srcHeight, uint32_t destWidth, uint32_t destHeight);

			void SendFrame(SmartPtrAVFrame& frame, double timeDurationInSeconds);

			SmartPtrAVFrame m_scaleSrcFrame;
			SmartPtrAVFrame m_scaleDstFrame;
			SmartPtrAVStream m_videoStream;
			SmartPtrAVFormatContext m_formatContext;

			SmartPtrSwsContext m_convertContext;

			uint32_t m_frameBufferSize;
			SmartPtrAvMalloc m_frameBuffer;

			uint32_t m_videoFrameWidth, m_videoFrameHeight;
			uint32_t m_scaleSrcImageWidth, m_scaleSrcImageHeight;
			Dimension2   _scalingImageSize;

			//uint64_t _lastTimeStamp;
			//bool _waitForFirstFrame;

			VideoSender::Statistic _statistic;

			uint32_t m_fps;
	};
}
