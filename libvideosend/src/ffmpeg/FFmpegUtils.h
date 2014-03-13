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

	//class SmartPtrAVFrame : public std::unique_ptr<AVFrame, void(*)(AVFrame*)>
	//{
	//	public:
	//		SmartPtrAVFrame() : std::unique_ptr<AVFrame, void(*)(AVFrame*)>(nullptr, deleter) { }
	//		SmartPtrAVFrame(AVFrame* newFrame) : std::unique_ptr<AVFrame, void(*)(AVFrame*)>(newFrame, deleter) { }
	//		SmartPtrAVFrame(SmartPtrAVFrame&& moved) : std::unique_ptr<AVFrame, void(*)(AVFrame*)>(std::move(moved)) { }
	//		void operator= (SmartPtrAVFrame&& moved)
	//		{
	//			std::unique_ptr<AVFrame, void(*)(AVFrame*)> &pObj = (*this);
	//			pObj = std::move(moved);
	//		}

	//		static void deleter(AVFrame*);

	//	private:
	//		SmartPtrAVFrame(const SmartPtrAVFrame&);
	//		const SmartPtrAVFrame& operator= (const SmartPtrAVFrame&);
	//};

	class FFmpegUtils
	{
		public:
			static AVFormatContext* createFormatContext(const std::string& outputType, enum AVCodecID codecId);

			static void copyRgbaToFrame(const uint8_t* srcData, const Dimension2& sourceSize, uint32_t stride, SmartPtrAVFrame& destFrame);

			static SmartPtrAVFrame createVideoFrame(enum PixelFormat pixFormat, uint32_t width, uint32_t height);

			static bool isConversionSupported(enum PixelFormat srcFmt, enum PixelFormat destFmt);

		private:
			FFmpegUtils() {}
	};
}
