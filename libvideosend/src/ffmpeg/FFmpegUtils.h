#pragma once

#include <libgrabber/src/Dimension.h>
#include "ffmpeg_includes.h"

#include <cstdint>
#include <iostream>

namespace dz
{
	class FFmpegUtils
	{
		public:
			static AVFormatContext* createFormatContext(const std::string& outputType, enum AVCodecID codecId);

			static void copyRgbaToFrame(const uint8_t* srcData, const Dimension2& sourceSize, uint32_t stride, AVFrame* destFrame);

			static AVFrame* createVideoFrame(enum PixelFormat pixFormat, const Dimension2& frameSize);
			static AVFrame* createVideoFrame(enum PixelFormat pixFormat, int width, int height);

			static bool isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt);

		private:
			FFmpegUtils() {}
	};
}
