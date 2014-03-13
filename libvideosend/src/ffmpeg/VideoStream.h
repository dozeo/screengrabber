#pragma once

#include <libgrabber/src/Dimension.h>
#include "VideoSender.h"
#include "ffmpeg_includes.h"
#include "FFmpegUtils.h"

#include <iostream>
#include <memory>

namespace dz
{
	class VideoStream
	{
		public:
			VideoStream(const Dimension2& videoSize, enum AVCodecID videoCodec);
			virtual ~VideoStream();

			void openUrl(const std::string& url, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level);
			void openFile(const std::string& filename, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level);

			void close();

			void sendFrame(const uint8_t* rgba, const Dimension2& imageSize, uint32_t stride, double timeDurationInSeconds);

			const VideoSender::Statistic* statistic() const { return &_statistic; }

		private:
			enum ConnectionMode 
			{
				CM_FILE = 0,
				CM_RTP,
			};

			void OpenVideoStream(const std::string& fileUrl, enum ConnectionMode mode, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level);

			AVStream* addVideoStream(enum AVCodecID codecId, int bitRate, int keyframe, float fps, enum PixelFormat pixFormat, enum VideoQualityLevel level);
			void openVideo(AVStream* stream);

			void setBasicSettings(AVCodecContext* codec, int bitRate, int keyframe, float fps, enum AVCodecID codecId, enum PixelFormat pixFormat, enum VideoQualityLevel level);

			void setupScaleContext(const Dimension2& srcSize, const Dimension2& destSize);
			void releaseScaleContext();

			void openStream(AVFormatContext* formatContext, const std::string& url, enum ConnectionMode mode);

			void closeVideo();
			void closeFile(AVFormatContext* formatContext);

			void sendFrame(SmartPtrAVFrame& frame, double timeDurationInSeconds);

			static const std::string StreamProtocol;
			static bool AVCodecInitialized;

			AVFormatContext* _formatContext;
			SwsContext* _convertContext;

			AVStream* _videoStream;
			SmartPtrAVFrame m_tempFrame;
			SmartPtrAVFrame m_scaledFrame;

			uint32_t _frameBufferSize;
			uint8_t* _frameBuffer;

			Dimension2   _videoFrameSize;
			Dimension2   _scalingImageSize;
			enum AVCodecID _videoCodec;

			uint64_t _lastTimeStamp;
			bool _waitForFirstFrame;

			bool _isStreamOpen;
			VideoSender::Statistic _statistic;
	};
}
