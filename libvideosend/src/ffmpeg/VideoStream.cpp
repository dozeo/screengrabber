#include "VideoStream.h"
#include "FFmpegUtils.h"
#include "../VideoSender.h"

#include <dzlib/dzexception.h>

#include <assert.h>
#include <string>

#define BITS_PER_PIXEL 32

#pragma warning(disable:4996)

void ffmpeg_log(void*, int line, const char* msg, va_list list)
{
	char buffer[2048];
	vsprintf(buffer, msg, list);
	std::cout << "[FFMPEG] - " << buffer;
}

//void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));

namespace dz
{
	const std::string VideoStream::StreamProtocol = std::string("flv");
	bool  VideoStream::AVCodecInitialized = false;

	VideoStream::VideoStream(const Dimension2& videoSize, enum AVCodecID videoCodec): _formatContext(NULL), _convertContext(NULL), _videoStream(NULL), _frameBufferSize(0),
		_frameBuffer(NULL), _videoFrameSize(videoSize), _videoCodec(videoCodec), _lastTimeStamp(0), _waitForFirstFrame(true), _isStreamOpen(false)/*, 
		m_tempFrame(nullptr, avframe_deleter), m_scaledFrame(nullptr, avframe_deleter)*/
	{
		if (!AVCodecInitialized)
		{
			av_register_all();
			avcodec_register_all();
			avformat_network_init();
			AVCodecInitialized = true;
		}

		//::av_log_set_callback(ffmpeg_log);
		//av_log_set_level(AV_LOG_DEBUG);
		::av_log_set_level(AV_LOG_ERROR);

		_scalingImageSize = _videoFrameSize;
		_videoFrameSize = videoSize;
		_videoFrameSize.width  -= _videoFrameSize.width % 4;
		_videoFrameSize.height -= _videoFrameSize.height % 4;
	}

	VideoStream::~VideoStream()
	{
		close();
	}

	void VideoStream::openUrl(const std::string& url, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level)
	{
		OpenVideoStream(url, CM_RTP, frameRate, bitRate, keyframe, level);
	}

	void VideoStream::openFile(const std::string& filename, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level)
	{
		OpenVideoStream(filename, CM_FILE, frameRate, bitRate, keyframe, level);
	}

	void VideoStream::OpenVideoStream(const std::string& fileUrl, enum ConnectionMode mode, float frameRate, int bitRate, int keyframe, enum VideoQualityLevel level)
	{
		_lastTimeStamp = 0;
		_waitForFirstFrame = true;

		PixelFormat destPixFormat = PIX_FMT_YUV420P;
		_formatContext = FFmpegUtils::createFormatContext(StreamProtocol, _videoCodec);
		if (_formatContext == 0)
			throw exception(strstream() << "VideoCodec not found. _videoCodec = " << _videoCodec);

		// add video stream
		if (_videoCodec != CODEC_ID_NONE)
		{
			_videoStream = addVideoStream(_videoCodec, bitRate, keyframe, frameRate, destPixFormat, level);

			AVCodecContext* ctx = _videoStream->codec;
			AVCodec* codec = avcodec_find_encoder(ctx->codec_id);
			if (!codec)
				throw exception(strstream() << "avcodec_find_encoder failed to find the encoder with id " << ctx->codec_id);

			int code = avcodec_open2(ctx, codec, NULL);
			if (code < 0)
				throw exception(strstream() << "avcodec_open2 failed with error code " << code << " to open the stream");
	
			_frameBufferSize = avpicture_get_size(ctx->pix_fmt, ctx->width, ctx->height);
			_frameBuffer = (uint8_t*)av_malloc(_frameBufferSize);
		}

		setupScaleContext(_scalingImageSize, _videoFrameSize);

		m_scaledFrame = std::move(FFmpegUtils::createVideoFrame(destPixFormat, _videoFrameSize.width, _videoFrameSize.height));
		openStream(_formatContext, fileUrl, mode);

		_isStreamOpen = true;
	}

	AVStream* VideoStream::addVideoStream(enum AVCodecID codecId, int bitRate, int keyframe, float fps, enum PixelFormat pixFormat, enum VideoQualityLevel level)
	{
		assert(_formatContext != 0);
		assert(codecId != CODEC_ID_NONE);

		AVCodec* codec = avcodec_find_encoder(_videoCodec);
		if (!codec)
			throw exception(strstream() << "avcodec_find_encoder in addVideoStream failed to find the encoder with id " << _videoCodec);

		AVStream* stream = avformat_new_stream(_formatContext, codec);
		if (!stream)
			throw exception(strstream() << "avformat_new_stream in addVideoStream failed");

		AVCodecContext* context = stream->codec;

		avcodec_get_context_defaults3(context, codec);
	
		setBasicSettings(context, bitRate, keyframe, fps, codecId, pixFormat, level);

		if (_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
			context->flags |= CODEC_FLAG_GLOBAL_HEADER;

		return stream;
	}

	void VideoStream::setBasicSettings(AVCodecContext* codec, int bitRate, int keyframe, float fps, enum AVCodecID codecId, enum PixelFormat pixFormat, enum VideoQualityLevel level)
	{
		const uint32_t keyframeEverySeconds = 4;
		const double refSec = 0.2;
		const uint32_t bframes = (uint32_t)(refSec / (1.0 / (double)fps));

		// set up properties
		codec->codec_type = AVMEDIA_TYPE_VIDEO;
		codec->coder_type = FF_CODER_TYPE_AC;//FF_CODER_TYPE_VLC;
		codec->width = _videoFrameSize.width;
		codec->height = _videoFrameSize.height;
		codec->bit_rate = bitRate;
		codec->pix_fmt = pixFormat;

		codec->codec_id = codecId;
		codec->time_base.den = (int)fps;
		codec->time_base.num = 1;
		codec->gop_size = 2*keyframe; // max key frames
		codec->keyint_min = keyframe; // minimum number of keyframes

		codec->me_method = ME_EPZS; // motion estimation algorithm
		codec->me_subpel_quality = 10;
		codec->delay = 0;
		codec->thread_count = 1; // determines the number of threads automatically
		codec->refs = 3;
		codec->max_b_frames = bframes;
		codec->rc_buffer_size = 0;

		// documentation says to set it to 2 on H.264
		codec->ticks_per_frame = 2;

		//codec->bit_rate = 500*1000;
		//codec->bit_rate_tolerance = 0;
		//codec->compression_level = 10;
		//codec->rc_max_rate = 0;
		//codec->rc_buffer_size = 0;
		codec->gop_size = fps * keyframeEverySeconds;
		codec->keyint_min = fps * keyframeEverySeconds;
		codec->max_b_frames = 2;
		codec->b_frame_strategy = 1;
		//codec->coder_type = 1;
		codec->me_cmp = 1;
		//codec->me_range = 16;
		//codec->qmin = 10;
		//codec->qmax = 51;
		//codec->scenechange_threshold = 40;
		//codec->flags |= CODEC_FLAG_LOOP_FILTER;
		//codec->me_method = ME_LOG;
		//codec->me_subpel_quality = 10;
		//codec->i_quant_factor = 0.71f;
		//codec->qcompress = 0.6f;
		//codec->max_qdiff = 4;
		//codec->directpred = 1;
		//codec->flags2 |= CODEC_FLAG2_FASTPSKIP;

		av_opt_set(codec->priv_data, "tune", "zerolatency", 0);
		av_opt_set(codec->priv_data, "profile", "high", 0);
		//av_opt_set(codec->priv_data, "vprofile", "baseline", 0);
		av_opt_set(codec->priv_data, "preset", "ultrafast", 0);

		//std::cout << "Video Quality: " << level << std::endl;

		/*if (level == VQ_LOW) {
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
		}*/
	}

	void VideoStream::openVideo(AVStream* stream)
	{
	}

	void VideoStream::close()
	{
		m_scaledFrame = nullptr;
		m_tempFrame = nullptr;

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

	void VideoStream::sendFrame(const uint8_t* rgba, const Dimension2& imageSize, uint32_t stride, double timeDurationInSeconds)
	{
		assert(_videoStream != 0);
		assert(m_scaledFrame != nullptr);
		assert(m_tempFrame != nullptr);

		if (imageSize != _scalingImageSize)
		{
			releaseScaleContext();
			setupScaleContext(imageSize, _videoFrameSize);
		}

		int64_t t0 = av_gettime();
		FFmpegUtils::copyRgbaToFrame(rgba, imageSize, stride, m_tempFrame);
		int scaleRes = sws_scale(_convertContext, m_tempFrame->data, m_tempFrame->linesize, 0, imageSize.height, m_scaledFrame->data, m_scaledFrame->linesize);
		if (scaleRes != _videoFrameSize.height)
			throw exception(strstream() << "Failed to scale the resulting frame from " << imageSize.width << "x" << imageSize.height << " to " << _videoFrameSize.width << "x" << _videoFrameSize.height);
		
		_statistic.lastScaleTime = (av_gettime() - t0);
		
		sendFrame(m_scaledFrame, timeDurationInSeconds);
	}

	void VideoStream::sendFrame(SmartPtrAVFrame& frame, double timeDurationInSeconds)
	{
		AVCodecContext* codec = _videoStream->codec;

		uint64_t timeStamp = (uint64_t)(timeDurationInSeconds * codec->time_base.den);

		if (_lastTimeStamp == timeStamp && !_waitForFirstFrame)
		{
			// ignore
			_statistic.lastEncodeTime = 0;
			_statistic.lastSendTime   = 0;
			_statistic.frameWritten(0);
			return;
		}

		_waitForFirstFrame = false;
		_lastTimeStamp = timeStamp;
		frame->pts = timeStamp;

		frame->pts = timeStamp;
		int64_t t0 = av_gettime();
		int size = avcodec_encode_video(codec, _frameBuffer, _frameBufferSize, frame.get());
		int64_t t1 = av_gettime();
		_statistic.lastEncodeTime = (t1 - t0);
		
		if (size > 0)
		{
			AVPacket packet;
			av_init_packet(&packet);

			if (codec->coded_frame->pts != AV_NOPTS_VALUE)
			{
				packet.pts = av_rescale_q(timeStamp, codec->time_base, _videoStream->time_base);
			}

			if (codec->coded_frame->key_frame)
			{
				packet.flags |= AV_PKT_FLAG_KEY;
			}

			packet.stream_index = _videoStream->index;
			packet.data = _frameBuffer;
			packet.size = size;
			packet.dts  = AV_NOPTS_VALUE;
		
			t0 = av_gettime();
			int result = av_interleaved_write_frame(_formatContext, &packet);
			if (result < 0)
				throw exception(strstream() << "av_interleaved_write_frame failed with return code: " << result);
			
			_statistic.lastSendTime = (av_gettime() - t0);
			int64_t bytes = packet.size;

			for (int i = 0; i < packet.side_data_elems; i++)
				bytes+= packet.side_data[i].size;
			
			_statistic.frameWritten(bytes);

			std::cout << "bytes send " << bytes << std::endl;
		}
		else
		{
			_statistic.lastSendTime = 0;
			_statistic.frameWritten (0);
		}
	}

	void VideoStream::setupScaleContext(const Dimension2& srcSize, const Dimension2& destSize)
	{
		assert(m_tempFrame == NULL);
		assert(_convertContext == NULL);

		PixelFormat srcFormat  = (BITS_PER_PIXEL == 24) ? AV_PIX_FMT_BGR24 : AV_PIX_FMT_BGRA;
		PixelFormat destFormat = PIX_FMT_YUV420P;

		if (!FFmpegUtils::isConversionSupported(srcFormat, destFormat))
			throw exception(strstream() << "color space conversion not supported");

		m_tempFrame = FFmpegUtils::createVideoFrame(srcFormat, srcSize.width, srcSize.height);
		_scalingImageSize = srcSize;

		_convertContext = sws_getContext(
			srcSize.width,
			srcSize.height,
			srcFormat,
			destSize.width,
			destSize.height,
			destFormat,
			SWS_FAST_BILINEAR/*SWS_BICUBIC*/,
			NULL,
			NULL,
			NULL);

		if (_convertContext == NULL)
			throw exception(strstream() << "sws_getContext failed (src " << srcSize.width << "x" << srcSize.height << ") (dst " << destSize.width << "x" << destSize.height << ")");
	}

	void VideoStream::releaseScaleContext()
	{
		if (_convertContext != 0)
		{
			sws_freeContext(_convertContext);
			_convertContext = 0;
		}
	}

	void VideoStream::closeVideo()
	{
		if (_videoStream != NULL)
		{
			if (_videoStream->codec->codec != 0)
				avcodec_close(_videoStream->codec);

			av_free(_frameBuffer);
			_frameBufferSize = 0;
			_frameBuffer = 0;
			_videoStream = 0;
		}
	}

	/// returns protocol of an url, or "" in case of an error
	static std::string urlGetProtocol (const std::string & url)
	{
		size_t pos = url.find ("://");
		if (pos == url.npos) return std::string();
		return url.substr (0, pos);
	}

	/// returns path of an url of the form [protocol]://[host][:port]/[path]?[searchpath]
	static std::string urlGetPath (const std::string & url)
	{
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

	void VideoStream::openStream(AVFormatContext* formatContext, const std::string& url, enum ConnectionMode mode)
	{
		AVIOContext* ioContext = 0;

		if (mode == CM_RTP)
		{
			AVDictionary* options = 0;
			// av_dict_set(&options, "rtsp_transport", "udp", 0);
			int result = avio_open2(&ioContext, url.c_str(), AVIO_FLAG_WRITE, NULL, &options);
			if (result < 0)
				throw exception(strstream() << "Failed to open url stream (" << url << ") with avio_open2 result = " << result );
		}
		else if (mode == CM_FILE)
		{
			int result = avio_open2(&ioContext, url.c_str(), AVIO_FLAG_WRITE, NULL, NULL);
			if (result < 0)
				throw exception(strstream() << "Failed to open video file (" << url << ") with avio_open2 result = " << result );
		}

		if (urlGetProtocol (url) == "tcp")
		{
			// set stream name if given as path string tcp://host:port/[streamName]
			std::string streamName = urlGetPath (url);
			if (!streamName.empty()){
				av_dict_set (&formatContext->metadata, "streamName", streamName.c_str() + 1, 0);
			}
		}

		formatContext->pb = ioContext;

		if (avformat_write_header(formatContext, NULL) < 0)
			throw exception(strstream() << "avformat_write_header failed to write header");
	}

	void VideoStream::closeFile(AVFormatContext* formatContext)
	{
		if (formatContext != 0 && _isStreamOpen)
		{
			av_write_trailer(formatContext);
			avio_close(formatContext->pb);
		}
	}
}
