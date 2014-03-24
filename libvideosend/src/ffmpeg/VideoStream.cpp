#include "VideoStream.h"
#include "FFmpegUtils.h"
#include "../VideoSender.h"

#include <dzlib/dzexception.h>

#include <assert.h>
#include <string>

#include <librtmp/amf.h>

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
	const PixelFormat OutputPixelFormat = PIX_FMT_YUV420P;
	const std::string StreamProtocol = std::string("flv");

	/// returns protocol of an url, or "" in case of an error
	static std::string urlGetProtocol(const std::string & url)
	{
		size_t pos = url.find ("://");
		if (pos == url.npos)
			return std::string();
		return url.substr (0, pos);
	}

	/// returns path of an url of the form [protocol]://[host][:port]/[path]?[searchpath]
	static std::string urlGetPath(const std::string & url)
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

	void SmartPtrAVStream::deleter(AVStream* object)
	{
		if (object && object->codec && object->codec)
			avcodec_close(object->codec);
	}

	void SmartPtrSwsContext::deleter(SwsContext* object)
	{
		if (object != nullptr)
			sws_freeContext(object);
	}

	void SmartPtrAvMalloc::deleter(uint8_t* memory)
	{
		if (memory != nullptr)
			av_free(memory);
	}

	VideoStream::VideoStream(const std::string& url, uint32_t width, uint32_t height, VideoQualityLevel::Enum level): m_frameBufferSize(0),
		/*_lastTimeStamp(0), _waitForFirstFrame(true), */m_scaleSrcImageWidth(0), m_scaleSrcImageHeight(0)
	{
		static bool bAVCodecInitialized = false;
		if (bAVCodecInitialized == false)
		{
			av_register_all();
			avcodec_register_all();
			avformat_network_init();
			bAVCodecInitialized = true;
		}

		::av_log_set_callback(ffmpeg_log);
		av_log_set_level(AV_LOG_INFO);
		//::av_log_set_level(AV_LOG_ERROR);

		//_scalingImageSize.width = width;
		//_scalingImageSize.height = height;
		m_videoFrameWidth = width - width % 4;
		m_videoFrameHeight = height - height % 4;

		//_lastTimeStamp = 0;
		//_waitForFirstFrame = true;
		
		m_formatContext = FFmpegUtils::CreateFormatContext(StreamProtocol, CODEC_ID_H264);
		
		m_videoStream = addVideoStream(level);

		AVCodecContext* ctx = m_videoStream->codec;
		AVCodec* codec = avcodec_find_encoder(ctx->codec_id);
		if (!codec)
			throw exception(strstream() << "avcodec_find_encoder failed to find the encoder with id " << ctx->codec_id);

		int code = avcodec_open2(ctx, codec, NULL);
		if (code < 0)
			throw exception(strstream() << "avcodec_open2 failed with error code " << code << " to open the stream");
	
		m_frameBufferSize = avpicture_get_size(ctx->pix_fmt, ctx->width, ctx->height);
		m_frameBuffer = static_cast<uint8_t*>(av_malloc(m_frameBufferSize));

		m_scaleDstFrame = std::move(FFmpegUtils::CreateVideoFrame(OutputPixelFormat, m_videoFrameWidth, m_videoFrameHeight));

		AVIOContext* ioContext = nullptr;
		AVDictionary* options = nullptr;

		// av_dict_set(&options, "rtsp_transport", "udp", 0);
		int result = avio_open2(&ioContext, url.c_str(), AVIO_FLAG_WRITE, NULL, &options);
		if (result < 0)
			throw exception(strstream() << "Failed to open url stream (" << url << ") with avio_open2 result = " << result );

		if (urlGetProtocol(url) == "tcp")
		{
			// set stream name if given as path string tcp://host:port/[streamName]
			std::string streamName = urlGetPath(url);
			if (!streamName.empty())
				av_dict_set(&options, "streamName", streamName.c_str() + 1, 0);
		}

		m_formatContext->pb = ioContext;

		//std::string text = (strstream() << "letitgo");
		//av_dict_set(&options, "widthtest", text.c_str() + 1, 0);

		if (avformat_write_header(m_formatContext.get(), &options) < 0)
			throw exception(strstream() << "avformat_write_header failed to write header");
	}

	VideoStream::~VideoStream()
	{
		m_scaleDstFrame = nullptr;
		m_scaleSrcFrame = nullptr;
		m_videoStream = nullptr;

		m_convertContext = nullptr;

		m_formatContext = nullptr;
	}

	SmartPtrAVStream VideoStream::addVideoStream(VideoQualityLevel::Enum level)
	{
		AVCodec* codec = avcodec_find_encoder(CODEC_ID_H264);
		if (!codec)
			throw exception(strstream() << "avcodec_find_encoder in addVideoStream failed to find the encoder with id " << CODEC_ID_H264);

		SmartPtrAVStream stream = avformat_new_stream(m_formatContext.get(), codec);
		if (stream == nullptr)
			throw exception(strstream() << "avformat_new_stream in addVideoStream failed");

		AVCodecContext* codecContext = stream->codec;

		avcodec_get_context_defaults3(codecContext, codec);
	
		SetBasicSettings(codecContext, level);

		if (m_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
			codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;

		return std::move(stream);
	}

	void VideoStream::SetBasicSettings(AVCodecContext* codec, VideoQualityLevel::Enum level)
	{
		uint32_t keyframeEverySeconds = 4;
		uint32_t bitrate = 100 * 1000;
		uint32_t keyframes = 30;
		float maxDelay = 0.2f;
		m_fps = 5;

		switch (level)
		{
			case VideoQualityLevel::Low:
				bitrate = 100 * 1000;
				m_fps = 5;
				keyframeEverySeconds = 5;
				maxDelay = 0.5f;
				break;

			case VideoQualityLevel::Medium:
				bitrate = 200 * 1000;
				m_fps = 10;
				keyframeEverySeconds = 3;
				maxDelay = 0.5f;
				break;

			case VideoQualityLevel::High:
				bitrate = 500 * 1000;
				m_fps = 10;
				keyframeEverySeconds = 1;
				maxDelay = 0.5f;
				break;
		}

		// set up properties
		codec->codec_type = AVMEDIA_TYPE_VIDEO;
		codec->coder_type = FF_CODER_TYPE_AC;//FF_CODER_TYPE_VLC;
		codec->width = m_videoFrameWidth;
		codec->height = m_videoFrameHeight;
		codec->pix_fmt = OutputPixelFormat;

		codec->codec_id = CODEC_ID_H264;
		//codec->flags |= CODEC_FLAG_LOOP_FILTER;
		//codec->time_base.den = (int)fps;
		//codec->time_base.num = 1;
		//codec->gop_size = 2*keyframe; // max key frames
		//codec->keyint_min = keyframe; // minimum number of keyframes

		//codec->me_method = ME_HEX; // motion estimation algorithm
		//codec->me_subpel_quality = 7;
		//codec->delay = 0;
		codec->thread_count = 0; // determines the number of threads automatically
		codec->refs = 3;
		codec->profile = FF_PROFILE_H264_BASELINE;
		//codec->max_b_frames = bframes;
		//codec->rc_buffer_size = 182 * 1000;
		//codec->rc_initial_buffer_occupancy = 182 * 1000;

		// documentation says to set it to 2 on H.264
		//codec->ticks_per_frame = 2;

		//codec->bit_rate = 0;
		//codec->rc_min_rate = 500*1000;
		//codec->rc_max_rate = 600*1000;
		//codec->bit_rate_tolerance = 0;
		//codec->compression_level = 10;
		//codec->rc_max_rate = 0;
		//codec->rc_buffer_size = 0;
		//codec->gop_size = int(fps * (float)keyframeEverySeconds);
		//codec->keyint_min = int(fps * (float)keyframeEverySeconds);
		//codec->max_b_frames = 2;
		//codec->b_frame_strategy = 1;
		//codec->coder_type = 1;
		//codec->me_cmp = 1;
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

		//const double refSec = 0.2;
		const double timePerFrame = (1.0 / (double)m_fps);
		const uint32_t bframes = 0;//(timePerFrame > maxDelay) ? (uint32_t)(maxDelay / (1.0 / (double)m_fps)) : 0;

		codec->max_b_frames = bframes;
		codec->bit_rate = bitrate;
		codec->gop_size = int(m_fps * keyframeEverySeconds);
		codec->keyint_min = int(m_fps * keyframeEverySeconds);
		codec->time_base.den = (int)m_fps;
		codec->time_base.num = 1;

		av_opt_set(codec->priv_data, "tune", "zerolatency", 0);
		av_opt_set(codec->priv_data, "profile", "baseline", 0);
		//av_opt_set(codec->priv_data, "vprofile", "baseline", 0);
		av_opt_set(codec->priv_data, "preset", "medium", 0);
		//av_opt_set(codec->priv_data, "tune", "zerolatency", 1);
		//av_opt_set(codec->priv_data, "preset", "medium", 0);

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

	//static void put_amf_double(AVIOContext *pb, double d)
	//{
	//	avio_w8(pb, AMF_DATA_TYPE_NUMBER);
	//	avio_wb64(pb, av_double2int(d));
	//}

	void VideoStream::SendFrame(VideoFrameHandle videoFrame, double timeDurationInSeconds)
	{
		if (videoFrame == nullptr)
			return;
	
		const VideoFrame& frame = *videoFrame.get();

		const uint32_t curWidth = frame.GetWidth();
		const uint32_t curHeight = frame.GetHeight();
		const uint32_t stride = frame.GetStride();
		const uint8_t* pImageData = frame.GetData();

		if ((curWidth != m_scaleSrcImageWidth) || (curHeight != m_scaleSrcImageHeight))
		{
			setupScaleContext(curWidth, curHeight, m_videoFrameWidth, m_videoFrameHeight);
		}

		int64_t t0 = av_gettime();
		FFmpegUtils::CopyRgbaToFrame(pImageData, curHeight, stride, m_scaleSrcFrame);

		int scaleRes = sws_scale(m_convertContext.get(), m_scaleSrcFrame->data, m_scaleSrcFrame->linesize, 0, curHeight, m_scaleDstFrame->data, m_scaleDstFrame->linesize);
		if (scaleRes != m_videoFrameHeight)
			throw exception(strstream() << "Failed to scale the resulting frame from " << curWidth << "x" << curHeight << " to " << m_videoFrameWidth << "x" << m_videoFrameHeight);
		
		_statistic.lastScaleTime = (av_gettime() - t0);

		SendFrame(m_scaleDstFrame, timeDurationInSeconds);
	}

	float VideoStream::GetFPS() const
	{
		return static_cast<float>(m_fps);
	}

	void VideoStream::SendFrame(SmartPtrAVFrame& frame, double timeDurationInSeconds)
	{
		AVCodecContext* codec = m_videoStream->codec;

		uint64_t timeStamp = (uint64_t)(timeDurationInSeconds * codec->time_base.den);

		//if (_lastTimeStamp == timeStamp && !_waitForFirstFrame)
		//{
		//	// ignore
		//	_statistic.lastEncodeTime = 0;
		//	_statistic.lastSendTime   = 0;
		//	_statistic.frameWritten(0);
		//	return;
		//}

		//_waitForFirstFrame = false;
		//_lastTimeStamp = timeStamp;

		frame->pts = timeStamp;
		int64_t t0 = av_gettime();
		int size = avcodec_encode_video(codec, m_frameBuffer.get(), m_frameBufferSize, frame.get());
		_statistic.lastEncodeTime = (av_gettime() - t0);
		
		if (size > 0)
		{
			AVPacket packet;
			av_init_packet(&packet);

			if (codec->coded_frame->pts != AV_NOPTS_VALUE)
				packet.pts = av_rescale_q(timeStamp, codec->time_base, m_videoStream->time_base);

			if (codec->coded_frame->key_frame)
				packet.flags |= AV_PKT_FLAG_KEY;

			packet.stream_index = m_videoStream->index;
			packet.data = m_frameBuffer.get();
			packet.size = size;
			packet.dts  = AV_NOPTS_VALUE;
		
			t0 = av_gettime();
			int result = av_interleaved_write_frame(m_formatContext.get(), &packet);
			if (result < 0)
				throw exception(strstream() << "av_interleaved_write_frame failed with return code: " << result);
			
			_statistic.lastSendTime = (av_gettime() - t0);
			int64_t bytes = packet.size;

			for (int i = 0; i < packet.side_data_elems; i++)
				bytes+= packet.side_data[i].size;
			
			_statistic.frameWritten(bytes);

			//std::cout << "bytes send " << bytes << std::endl;
		}
		else
		{
			_statistic.lastSendTime = 0;
			_statistic.frameWritten (0);
		}
	}

	void VideoStream::setupScaleContext(uint32_t srcWidth, uint32_t srcHeight, uint32_t destWidth, uint32_t destHeight)
	{
		std::cout << "Setting up scale context from size " << srcWidth << "x" << srcHeight << " to " << destWidth << "x" << destHeight << std::endl;

		PixelFormat srcFormat  = (BITS_PER_PIXEL == 24) ? AV_PIX_FMT_BGR24 : AV_PIX_FMT_BGRA;

		if (!FFmpegUtils::isConversionSupported(srcFormat, OutputPixelFormat))
			throw exception(strstream() << "color space conversion not supported");

		m_scaleSrcFrame = FFmpegUtils::CreateVideoFrame(srcFormat, srcWidth, srcHeight);
		m_scaleSrcImageWidth = srcWidth;
		m_scaleSrcImageHeight = srcHeight;

		m_convertContext = sws_getContext(
			m_scaleSrcImageWidth,
			m_scaleSrcImageHeight,
			srcFormat,
			destWidth,
			destHeight,
			OutputPixelFormat,
			//SWS_FAST_BILINEAR, // or
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL);

		if (m_convertContext == nullptr)
			throw exception(strstream() << "sws_getContext failed (src " << m_scaleSrcImageWidth << "x" << m_scaleSrcImageHeight << ") (dst " << destWidth << "x" << destHeight << ")");
	}
}
