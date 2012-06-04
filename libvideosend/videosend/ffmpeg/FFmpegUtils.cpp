#include "FFmpegUtils.h"

#include <grabber/Math.h>

#include <assert.h>

namespace dz {

AVFormatContext* FFmpegUtils::createFormatContext(const std::string& outputType, enum CodecID codecId)
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

void FFmpegUtils::copyRgbaToFrame(
	const uint8_t* rgba,
	const Dimension2& sourceSize,
	uint32_t stride,
	AVFrame* destFrame)
{
	assert(rgba != NULL);
	assert(destFrame != NULL);

	uint8_t* srcData = const_cast<uint8_t*>(rgba);
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
	uint8_t* frameBuffer = NULL;

	frame = avcodec_alloc_frame();
	if (frame == NULL)
	{
		return NULL;
	}

	int size = avpicture_get_size(pixFormat, width, height);
	frameBuffer = (uint8_t*)av_malloc(size);
	if (frameBuffer == 0)
	{
		return NULL;
	}

	avpicture_fill((AVPicture*)frame, frameBuffer, pixFormat, width, height);

	return frame;
}

bool FFmpegUtils::isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt)
{
	return (sws_isSupportedInput(srcFmt) > 0) && (sws_isSupportedOutput(destFmt) > 0);
}

}