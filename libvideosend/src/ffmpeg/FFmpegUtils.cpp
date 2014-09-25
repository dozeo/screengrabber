#include "FFmpegUtils.h"

#include <libcommon/math_helpers.h>
#include <assert.h>

#include <dzlib/dzexception.h>

namespace dz
{
	void SmartPtrAVFormatContext::deleter(AVFormatContext* context)
	{
		if (context)
		{
			if (context->pb)
				av_write_trailer(context);
			avio_close(context->pb);

			for (unsigned int i = 0; i < context->nb_streams; i++)
			{
				av_freep(&context->streams[i]->codec);
				av_freep(&context->streams[i]);
			}
			
			av_free(context);
		}
	}

	void SmartPtrAVFrame::deleter(AVFrame* frame)
	{
		if (frame && frame->data[0])
			avpicture_free((AVPicture*)frame);

		avcodec_free_frame(&frame);
	}

	SmartPtrAVFormatContext FFmpegUtils::CreateFormatContext(const std::string& outputType, enum AVCodecID codecId)
	{
		AVOutputFormat* outputFormat = av_guess_format(outputType.c_str(), NULL, NULL);
		if (outputFormat == nullptr)
			throw dz::exception(strstream() << "av_guess_format(" << outputType << ") failed to find an output format");

		AVFormatContext* formatContext = nullptr;
		int iError = avformat_alloc_output_context2(&formatContext, outputFormat, NULL, NULL);
		if (iError < 0 || formatContext == nullptr)
			throw dz::exception(strstream() << "avformat_alloc_output_context2() failed to create a format context with error code " << iError);

		formatContext->oformat = outputFormat;
		formatContext->video_codec_id = codecId;
		formatContext->max_delay = 0;

		return SmartPtrAVFormatContext(formatContext);
	}

	void FFmpegUtils::CopyRgbaToFrame(const uint8_t* srcData, uint32_t height, uint32_t stride, SmartPtrAVFrame& destFrame)
	{
		assert(srcData != NULL);
		assert(destFrame != NULL);

		uint8_t* destData = destFrame->data[0];
		assert( destFrame->linesize[0] > 0 );
		assert( destData != NULL );
		uint32_t lineSize = (uint32_t)destFrame->linesize[0];

		if (lineSize == stride)
		{
			memcpy(destData, srcData, stride * height);
		}
		else
		{
			uint32_t copyStride = stride;
			if (copyStride > lineSize)
				copyStride = lineSize;

			for (uint32_t y = 0; y < height; y++)
			{
				memcpy(destData, srcData, copyStride);
				srcData += stride;
				destData += lineSize;
			}
		}
	}

	SmartPtrAVFrame FFmpegUtils::CreateVideoFrame(enum PixelFormat pixFormat, uint32_t width, uint32_t height)
	{
		//uint8_t* frameBuffer = NULL;
		
		SmartPtrAVFrame frame(avcodec_alloc_frame());
		if (frame == nullptr)
			throw dz::exception(strstream() << "avcodec_alloc_frame() failed with params pixFormat = " << pixFormat << ", width = " << width << ", height = " << height);
		
		//int size = avpicture_get_size(pixFormat, width, height);
		//if (size <= 0)
		//	throw dz::exception(strstream() << "avpicture_get_size() failed and gave size " << size << " with params pixFormat = " << pixFormat << ", width = " << width << ", height = " << height);

		//frameBuffer = (uint8_t*)av_malloc(size);
		//if (frameBuffer == NULL)
		//	throw dz::exception(strstream() << "av_malloc() for size " << size);

		//avpicture_fill((AVPicture*)frame.get(), frameBuffer, pixFormat, width, height);
		if (avpicture_alloc((AVPicture*)frame.get(), pixFormat, width, height) != 0)
			throw dz::exception(strstream() << "avpicture_alloc failed to allocate a picture for frame with size " << width << "x" << height << ".");

		return std::move(frame);
	}

	bool FFmpegUtils::isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt)
	{
		return (sws_isSupportedInput(srcFmt) > 0) && (sws_isSupportedOutput(destFmt) > 0);
	}
}