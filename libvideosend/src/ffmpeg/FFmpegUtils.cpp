#include "FFmpegUtils.h"

#include <libcommon/math_helpers.h>
#include <assert.h>

#include <dzlib/dzexception.h>

namespace dz
{
	AVFormatContext* FFmpegUtils::createFormatContext(const std::string& outputType, enum AVCodecID codecId)
	{
		AVFormatContext* formatContext = 0;

		AVOutputFormat* outputFormat = av_guess_format(outputType.c_str(), NULL, NULL);
		if (outputFormat == 0) {
			return NULL;
		}

		avformat_alloc_output_context2(&formatContext, outputFormat, NULL, NULL);
		if (formatContext == 0) {
			return NULL;
		}

		formatContext->oformat = outputFormat;
		formatContext->video_codec_id = codecId;
		formatContext->max_delay = 0;

		return formatContext;
	}

	void FFmpegUtils::copyRgbaToFrame(const uint8_t* srcData, const Dimension2& sourceSize, uint32_t stride, AVFrame* destFrame)
	{
		assert(srcData != NULL);
		assert(destFrame != NULL);

		uint8_t* destData = destFrame->data[0];
		assert( destFrame->linesize[0] > 0 );
		assert( destData != NULL );
		uint32_t lineSize = (uint32_t)destFrame->linesize[0];

		uint32_t copyStride = stride;
		if (copyStride > lineSize)
			copyStride = lineSize;

		for (int y = 0; y < sourceSize.height; y++)
		{
			memcpy(destData, srcData, copyStride);
			srcData += stride;
			destData += lineSize;
		}
	}

	AVFrame* FFmpegUtils::createVideoFrame(enum PixelFormat pixFormat, const Dimension2& frameSize)
	{
		return createVideoFrame(pixFormat, frameSize.width, frameSize.height);
	}

	AVFrame* FFmpegUtils::createVideoFrame(enum PixelFormat pixFormat, int width, int height)
	{
		AVFrame* frame = NULL;
		uint8_t* frameBuffer = NULL;
		
		frame = avcodec_alloc_frame();
		if (frame == NULL)
			throw dz::exception(strstream() << "avcodec_alloc_frame() failed with params pixFormat = " << pixFormat << ", width = " << width << ", height = " << height);
		
		int size = avpicture_get_size(pixFormat, width, height);
		if (size <= 0)
		{
			avcodec_free_frame(&frame);
			throw dz::exception(strstream() << "avpicture_get_size() failed and gave size " << size << " with params pixFormat = " << pixFormat << ", width = " << width << ", height = " << height);
		}

		frameBuffer = (uint8_t*)av_malloc(size);
		if (frameBuffer == NULL)
		{
			avcodec_free_frame(&frame);
			throw dz::exception(strstream() << "av_malloc() for size " << size);
		}

		avpicture_fill((AVPicture*)frame, frameBuffer, pixFormat, width, height);

		return frame;
	}

	bool FFmpegUtils::isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt)
	{
		return (sws_isSupportedInput(srcFmt) > 0) && (sws_isSupportedOutput(destFmt) > 0);
	}
}