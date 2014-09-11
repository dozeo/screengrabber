#include "FFmpegUtils.h"
#include <dzlib/dzexception.h>

#include <libgrabber/src/Math.h>
#include <assert.h>

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

	void FFmpegUtils::copyRgbaToFrame(const uint8_t* rgba, const Dimension2& sourceSize, uint32_t stride, AVFrame* destFrame)
	{
		assert(rgba != NULL);
		assert(destFrame != NULL);

		const uint8_t* srcData = rgba;
		uint8_t* destData = destFrame->data[0];
		int lineSize = destFrame->linesize[0];

		for (int y = 0; y < sourceSize.height; y++)
		{
			memcpy(destData, srcData, sourceSize.width * 4);
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

		frame = avcodec_alloc_frame();
		if (frame == NULL)
			return NULL;

        frame->format = pixFormat;
        frame->width = width;
        frame->height = height;
        if (av_frame_get_buffer(frame, 4) != 0)
            throw exception("av_frame_buffer() failed to allocate buffers");

		return frame;
	}

	bool FFmpegUtils::isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt)
	{
		return (sws_isSupportedInput(srcFmt) > 0) && (sws_isSupportedOutput(destFmt) > 0);
	}
}