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

			uint32_t GetWidth() const { return m_width; }
			uint32_t GetHeight() const { return m_height; }

			uint32_t GetStride() const;
			uint8_t* GetData() const;

			uint32_t GetPixelSize() const;
			VideoFrameFormat::Enum GetVideoFrameFormat() const { return m_format; }

		private:
			uint32_t m_width, m_height;
			VideoFrameFormat::Enum m_format;
			std::unique_ptr<uint8_t> m_pData;
	};
}