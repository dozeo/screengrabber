#pragma once

#include <cstdint>
#include <memory>

namespace dz
{
	namespace VideoFrameFormat
	{
		enum Enum
		{
			RGB,
			RGBA
		};
	};

	class VideoFrame
	{
		public:
			VideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format);
			~VideoFrame();

			void Clear();

			uint32_t GetWidth() const { return m_width; }
			uint32_t GetHeight() const { return m_height; }

			uint32_t GetStride() const;
			uint8_t* GetData() const;

			uint32_t GetPixelSize() const { return m_pixelSize; }
			VideoFrameFormat::Enum GetVideoFrameFormat() const { return m_format; }

		private:
			uint32_t m_width, m_height, m_pixelSize;
			VideoFrameFormat::Enum m_format;
			std::unique_ptr<uint8_t> m_pData;
	};

	class VideoFrameHandle : public std::unique_ptr<VideoFrame, void(*)(VideoFrame*)>
	{
		public:
			VideoFrameHandle(VideoFrame*handledFrame);
			VideoFrameHandle(VideoFrameHandle&& other);

		private:
			VideoFrameHandle(VideoFrameHandle&);

			static void VideoFrameDeleter(VideoFrame* frame);
	};
}