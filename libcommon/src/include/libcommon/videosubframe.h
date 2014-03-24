#pragma once

#include <cstdint>
#include <memory>

#include <libcommon/videoframe.h>
#include <libcommon/dzrect.h>

namespace dz
{
	class VideoSubframe
	{
		public:
			VideoSubframe(VideoFrame& frame, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

			void Clear();
			uint32_t GetWidth() const { return m_width; }
			uint32_t GetHeight() const { return m_height; }
			uint32_t GetPixelSize() const { return m_frame.GetPixelSize(); }
			uint32_t CountLines() const;
			uint32_t GetStride() const;
			uint32_t GetDataWidth() const;
			uint8_t* GetLineData(uint32_t lineNumber = 0);

		private:
			VideoFrame& m_frame;
			uint32_t m_x, m_y, m_width, m_height;
	};
}