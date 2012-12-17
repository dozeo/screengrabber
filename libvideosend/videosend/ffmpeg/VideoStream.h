#pragma once

#include <grabber/platform.h>
#include <grabber/Dimension.h>

#include "../VideoSender.h"
#include "ffmpeg_includes.h"

#include <iostream>

namespace dz {


class VideoStream
{
public:
	VideoStream(const Dimension2& videoSize, enum CodecID videoCodec);
	virtual ~VideoStream();

	int openUrl(const std::string& url, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level);
	int openFile(const std::string& filename, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level);

	void close();

	int sendFrame(const uint8_t* rgba, const Dimension2& imageSize, uint32_t stride, double timeDurationInSeconds);
	const VideoSender::Statistic * statistic () const { return &_statistic; }

private:

	enum ConnectionMode {
		CM_FILE = 0,
		CM_RTP,
	};

	int open(
		const std::string& fileUrl,
		enum ConnectionMode mode,
		float frameRate,
		int bitRate,
		int keyframe,
		enum VideoQualityLevel level);

	AVStream* addVideoStream(enum CodecID codecId, int bitRate, int keyframe, float fps, enum PixelFormat pixFormat, enum VideoQualityLevel level);
	int openVideo(AVStream* stream);

	void setBasicSettings(AVCodecContext* codec, int bitRate, int keyframe, float fps, enum CodecID codecId, enum PixelFormat pixFormat);
	void setVideoQualitySettings(AVCodecContext* codec, enum VideoQualityLevel level);

	int setupScaleContext(const Dimension2& srcSize, const Dimension2& destSize);
	void releaseScaleContext();

	int openStream(AVFormatContext* formatContext, const std::string& url, enum ConnectionMode mode);

	void closeVideo();
	void closeFile(AVFormatContext* formatContext);

	int sendFrame(AVStream* videoStream, AVFrame* frame, double timeDurationInSeconds);

	static const std::string StreamProtocol;
	static bool              AVCodecInitialized;

	AVFormatContext* _formatContext;
	SwsContext* _convertContext;

	AVStream* _videoStream;
	AVFrame*  _tempFrame;
	AVFrame*  _scaledFrame;

	uint32_t _frameBufferSize;
	uint8_t* _frameBuffer;

	Dimension2   _videoFrameSize;
	Dimension2   _scalingImageSize;
	enum CodecID _videoCodec;

	uint64_t _lastTimeStamp;
	bool _waitForFirstFrame;

	bool _isStreamOpen;
	VideoSender::Statistic    _statistic;
};

}
