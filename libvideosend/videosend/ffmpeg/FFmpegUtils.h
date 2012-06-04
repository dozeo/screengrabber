#pragma once

#include <grabber/platform.h>
#include <grabber/Dimension.h>

#include <iostream>

#include "ffmpeg_includes.h"


namespace dz {

class FFmpegUtils
{
public:
	static AVFormatContext* createFormatContext(const std::string& outputType, enum CodecID codecId);

	static void copyRgbaToFrame(const uint8_t* rgba, const Dimension2& sourceSize, uint32_t stride, AVFrame* destFrame);

	static AVFrame* createVideoFrame(enum PixelFormat pixFormat, const Dimension2& frameSize);
	static AVFrame* createVideoFrame(enum PixelFormat pixFormat, int width, int height);

	static bool isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt);

private:

	FFmpegUtils() {}
};

}
