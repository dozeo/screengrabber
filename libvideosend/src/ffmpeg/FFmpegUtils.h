#pragma once

#include <libgrabber/src/Dimension.h>
#include "ffmpeg_includes.h"

#include <cstdint>
#include <iostream>
#include <memory>

namespace dz
{
	//void avframe_deleter(AVFrame* frame);
	//typedef std::unique_ptr<AVFrame, void(*)(AVFrame*)> SmartPtrAVFrame;

	template<typename T>
	class UniquePtrCustom : public std::unique_ptr<T, void(*)(T*)>
	{
		public:
			typedef std::unique_ptr<T, void(*)(T*)> uniqueptr;

			UniquePtrCustom() : uniqueptr(nullptr, deleter) { }
			UniquePtrCustom(T* newPtr) : uniqueptr(newPtr, deleter) { }
			UniquePtrCustom(UniquePtrCustom&& moved) : uniqueptr(std::move(moved)) { }

			void operator= (UniquePtrCustom&& moved)
			{
				uniqueptr &pObj = (*this);
				pObj = std::move(moved);
			}

			static void deleter(T*);

		private:
			UniquePtrCustom(const UniquePtrCustom&);
			const UniquePtrCustom& operator= (const UniquePtrCustom&);
	};

	typedef UniquePtrCustom<AVFrame> SmartPtrAVFrame;
	typedef UniquePtrCustom<AVFormatContext> SmartPtrAVFormatContext;

	class FFmpegUtils
	{
		public:
			static SmartPtrAVFormatContext CreateFormatContext(const std::string& outputType, enum AVCodecID codecId);

			static void CopyRgbaToFrame(const uint8_t* srcData, uint32_t height, uint32_t stride, SmartPtrAVFrame& destFrame);

			static SmartPtrAVFrame CreateVideoFrame(enum PixelFormat pixFormat, uint32_t width, uint32_t height);

			static bool isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt);

		private:
			FFmpegUtils() {}
	};
}
