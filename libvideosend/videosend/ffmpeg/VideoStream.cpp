
#include "VideoStream.h"
#include "FFmpegUtils.h"
#include "../VideoSender.h"

#include <assert.h>
#include <string>

namespace dz {

const std::string VideoStream::StreamProtocol     = std::string("flv");
bool              VideoStream::AVCodecInitialized = false;

VideoStream::VideoStream(const Dimension2& videoSize, enum CodecID videoCodec)
: _formatContext(NULL)
, _convertContext(NULL)
, _videoStream(NULL)
, _tempFrame(NULL)
, _scaledFrame(NULL)
, _frameBufferSize(0)
, _frameBuffer(NULL)
, _videoFrameSize(videoSize)
, _videoCodec(videoCodec)
, _lastTimeStamp(0)
, _isStreamOpen(false)
{
	if (!AVCodecInitialized) {
		av_register_all();
		avcodec_register_all();
		avformat_network_init();
		AVCodecInitialized = true;
	}

	_videoFrameSize.width  = videoSize.width  - (videoSize.width % 4);
	_videoFrameSize.height = videoSize.height - (videoSize.height % 4);
	_scalingImageSize = _videoFrameSize;
}

VideoStream::~VideoStream()
{
	close();
}

int VideoStream::openUrl(const std::string& url, float frameRate, int bitRate, enum VideoQualityLevel level)
{
	return open(url, CM_RTP, frameRate, bitRate, level);
}

int VideoStream::openFile(const std::string& filename, float frameRate, int bitRate, enum VideoQualityLevel level)
{
	return open(filename, CM_FILE, frameRate, bitRate, level);
}

int VideoStream::open(
	const std::string& fileUrl,
	enum ConnectionMode mode,
	float frameRate,
	int bitRate,
	enum VideoQualityLevel level)
{
	int result = 0;
	_lastTimeStamp = 0;

	PixelFormat destPixFormat = PIX_FMT_YUV420P;
	_formatContext = FFmpegUtils::createFormatContext(StreamProtocol, _videoCodec);
	if (_formatContext == 0) {
		return VideoSender::VE_CODEC_NOT_FOUND;
	}

	// add video stream
	if (_videoCodec != CODEC_ID_NONE) {
		_videoStream = addVideoStream(_videoCodec, bitRate, frameRate, destPixFormat, level);
		if (_videoStream == 0) {
			close();
			return VideoSender::VE_FAILED_OPEN_STREAM;
		}
		result = openVideo(_videoStream);
		if (result != VideoSender::VE_OK) {
			close();
			return result;
		}
	}

	result = setupScaleContext(_scalingImageSize, _videoFrameSize);
	if (result) {
		close();
		return result;
	}

	_scaledFrame = FFmpegUtils::createVideoFrame(destPixFormat, _videoFrameSize);
	result = openStream(_formatContext, fileUrl, mode);
	if (result) {
		close();
		return result;
	}

	_isStreamOpen = true;

	return VideoSender::VE_OK;
}

AVStream* VideoStream::addVideoStream(
	enum CodecID codecId,
	int bitRate,
	float fps,
	enum PixelFormat pixFormat,
	enum VideoQualityLevel level)
{
	assert(_formatContext != 0);
	assert(codecId != CODEC_ID_NONE);

	AVCodec* codec = avcodec_find_encoder(_videoCodec);
	if (codec < 0) {
		return NULL;
	}

	AVStream* stream = avformat_new_stream(_formatContext, codec);
	if (stream == 0) {
		return NULL;
	}
	AVCodecContext* context = stream->codec;

	avcodec_get_context_defaults3(context, codec);
	
	setBasicSettings(context, bitRate, fps, codecId, pixFormat);
	setVideoQualitySettings(context, level);

	if (_formatContext->oformat->flags & AVFMT_GLOBALHEADER) {
		context->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	return stream;
}

void VideoStream::setBasicSettings(
	AVCodecContext* codec,
	int bitRate,
	float fps,
	enum CodecID codecId,
	enum PixelFormat pixFormat)
{
	// set up properties
	codec->codec_type  = AVMEDIA_TYPE_VIDEO;
	codec->coder_type  = FF_CODER_TYPE_VLC;
	codec->width       = _videoFrameSize.width;
	codec->height      = _videoFrameSize.height;
	codec->bit_rate    = bitRate;
	codec->pix_fmt     = pixFormat;

	codec->codec_id      = codecId;
	codec->time_base.den = fps;
	codec->time_base.num = 1;
	codec->gop_size      = 10;  // max key frames
}

void VideoStream::setVideoQualitySettings(AVCodecContext* codec, enum VideoQualityLevel level) {

	codec->me_method = 7; //motion estimation algorithm
	codec->me_subpel_quality = 4;
	codec->delay = 0;
	codec->thread_count = 1;
	codec->refs = 3;
	codec->rc_buffer_size = 0;

	std::cout << "Video Quality: " << level << std::endl;

	if (level == VQ_LOW) {
		codec->max_b_frames = 0;
	
		av_opt_set(codec->priv_data, "profile", "baseline", 0);
		av_opt_set(codec->priv_data, "preset", "veryfast", 0);
		av_opt_set(codec->priv_data, "tune", "zerolatency", 0);
	} else if (level == VQ_MEDIUM) {
		codec->max_b_frames = 0;
	
		av_opt_set(codec->priv_data, "profile", "baseline", 0);
		av_opt_set(codec->priv_data, "preset", "fast", 0);
		av_opt_set(codec->priv_data, "tune", "zerolatency", 0);
	} else {
		codec->max_b_frames = 2;
		av_opt_set(codec->priv_data, "profile", "main", 0);
		av_opt_set(codec->priv_data, "preset", "medium", 0);
		av_opt_set(codec->priv_data, "tune", "film", 0);
	}
}

int VideoStream::openVideo(AVStream* stream) {
	assert(stream != 0);

	AVCodecContext* ctx = stream->codec;
	AVCodec* codec = avcodec_find_encoder(ctx->codec_id);
	if (!codec) {
		return VideoSender::VE_CODEC_NOT_FOUND;
	}

	if (avcodec_open(ctx, codec) < 0) {
		return VideoSender::VE_FAILED_OPEN_STREAM;
	}
	
	_frameBufferSize = avpicture_get_size(ctx->pix_fmt, ctx->width, ctx->height);
	_frameBuffer = (uint8_t*)av_malloc(_frameBufferSize);

	return VideoSender::VE_OK;
}

void VideoStream::close() {
	closeFile(_formatContext);
	closeVideo();

	releaseScaleContext();

	if (_formatContext != NULL) {
		for (unsigned int i = 0; i < _formatContext->nb_streams; i++) {
			av_freep(&_formatContext->streams[i]->codec);
			av_freep(&_formatContext->streams[i]);
		}
		av_free(_formatContext);
		_formatContext = NULL;
	}

	_isStreamOpen = false;
}

int VideoStream::sendFrame(
	const uint8_t* rgba,
	const Dimension2& imageSize,
	uint32_t stride,
	double timeDurationInSeconds)
{
	assert(_videoStream != 0);
	assert(_scaledFrame != 0);
	assert(_tempFrame != 0);

	if (imageSize != _scalingImageSize) {
		releaseScaleContext();
		setupScaleContext(imageSize, _videoFrameSize);
	}

	FFmpegUtils::copyRgbaToFrame(rgba, imageSize, stride, _tempFrame);
	sws_scale(
		_convertContext,
		_tempFrame->data, _tempFrame->linesize,
		0,
		imageSize.height,
		_scaledFrame->data, _scaledFrame->linesize);

	return sendFrame(_videoStream, _scaledFrame, timeDurationInSeconds);
}

int VideoStream::sendFrame(AVStream* videoStream, AVFrame* frame, double timeDurationInSeconds)
{
	int result = 0;
	AVCodecContext* codec = videoStream->codec;

	uint64_t timeStamp = (uint64_t)(timeDurationInSeconds * codec->time_base.den);
	if (_lastTimeStamp == timeStamp) {
		return result;
	}
	_lastTimeStamp = timeStamp;
	frame->pts = timeStamp;

	frame->pts = timeStamp;
	int size = avcodec_encode_video(codec, _frameBuffer, _frameBufferSize, frame);
	if (size > 0) {
		AVPacket packet;
		av_init_packet(&packet);

		if (codec->coded_frame->pts != AV_NOPTS_VALUE) {
			packet.pts = av_rescale_q(timeStamp, codec->time_base, _videoStream->time_base);
		}
		if (codec->coded_frame->key_frame) {
			packet.flags |= AV_PKT_FLAG_KEY;
		}

		packet.stream_index = _videoStream->index;
		packet.data = _frameBuffer;
		packet.size = size;
		packet.dts  = AV_NOPTS_VALUE;
		
		result = av_interleaved_write_frame(_formatContext, &packet);
	}

	return result;
}

int VideoStream::setupScaleContext(
	const Dimension2& srcSize,
	const Dimension2& destSize)
{
	assert(_tempFrame == NULL);
	assert(_convertContext == NULL);

	PixelFormat srcFormat  = PIX_FMT_BGRA;
	PixelFormat destFormat = PIX_FMT_YUV420P;

	if (!FFmpegUtils::isConversionSupported(srcFormat, destFormat)) {
		std::cerr << "color space conversion not supported" << std::endl;
		return VideoSender::VE_INVALID_CONVERSION;
	}

	_tempFrame = FFmpegUtils::createVideoFrame(srcFormat, srcSize);
	_scalingImageSize = srcSize;

	_convertContext = sws_getContext(
		srcSize.width,
		srcSize.height,
		srcFormat,
		destSize.width,
		destSize.height,
		destFormat,
		SWS_BICUBIC,
		NULL,
		NULL,
		NULL);
	if (_convertContext == NULL) {
		return VideoSender::VE_INVALID_CONVERSION;
	}
	return VideoSender::VE_OK;
}

void VideoStream::releaseScaleContext()
{
	if (_tempFrame != 0) {
		av_free(_tempFrame->data[0]);
		av_free(_tempFrame);
		_tempFrame = 0;
	}
	if (_convertContext != 0) {
		sws_freeContext(_convertContext);
		_convertContext = 0;
	}
}

void VideoStream::closeVideo()
{
	if (_videoStream != NULL) {
		if (_videoStream->codec->codec != 0) {
			avcodec_close(_videoStream->codec);
		}
		if (_scaledFrame != 0) {
			av_free(_scaledFrame->data[0]);
			av_free(_scaledFrame);
			_scaledFrame = 0;
		}

		av_free(_frameBuffer);
		_frameBufferSize = 0;
		_frameBuffer = 0;
		_videoStream = 0;
	}
}

/// returns protocol of an url, or "" in case of an error
static std::string urlGetProtocol (const std::string & url) {
	size_t pos = url.find ("://");
	if (pos == url.npos) return std::string();
	return url.substr (0, pos);
}

/// returns path of an url of the form [protocol]://[host]:[port]/[path]?[searchpath]
static std::string urlGetPath (const std::string & url) {
	size_t pos = url.find("://");
	if (pos == url.npos) return std::string (); // no protocol given
	pos = url.find ('/', pos + 3);
	if (pos == url.npos) return std::string (); // no path given
	size_t endpos = url.find ('?', pos + 1);
	if (endpos == url.npos) {
		return url.substr (pos);
	} else {
		return url.substr (pos, endpos - pos);
	}
}

int VideoStream::openStream(AVFormatContext* formatContext, const std::string& url, enum ConnectionMode mode)
{
	AVIOContext* ioContext = 0;

	if (mode == CM_RTP) {
		AVDictionary* options = 0;
		// av_dict_set(&options, "rtsp_transport", "udp", 0);
		int result = avio_open2(&ioContext, url.c_str(), AVIO_FLAG_WRITE, NULL, &options);
		if (result < 0) {
			std::cerr << "Failed to open stream (" << url << ") : " << result << std::endl;
			return VideoSender::VE_FAILED_OPEN_STREAM;
		}
	}
	else if (mode == CM_FILE) {
		int result = avio_open2(&ioContext, url.c_str(), AVIO_FLAG_WRITE, NULL, NULL);
		if (result < 0) {
			std::cerr << "Failed to open video file (" << url << ")" << std::endl;
			return VideoSender::VE_FAILED_OPEN_STREAM;
		}
	}
	if (urlGetProtocol (url) == "tcp") {
		// set stream name if given as path string tcp://host:port/[streamName]
		std::string streamName = urlGetPath (url);
		if (!streamName.empty()){
			av_dict_set (&formatContext->metadata, "streamName", streamName.c_str() + 1, 0);
		}
	}

	formatContext->pb = ioContext;

	if (avformat_write_header(formatContext, NULL) < 0) {
		std::cerr << "Failed to write header" << std::endl;
		return VideoSender::VE_FAILED_OPEN_STREAM;
	}

	return VideoSender::VE_OK;
}

void VideoStream::closeFile(AVFormatContext* formatContext)
{
	if (formatContext != 0 && _isStreamOpen) {
		av_write_trailer(formatContext);
		avio_close(formatContext->pb);
	}
}

}
