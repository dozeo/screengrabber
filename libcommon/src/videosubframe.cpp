#include <libcommon/videosubframe.h>
#include <dzlib/dzexception.h>
#include <slog/slog.h>

namespace dz
{
	VideoSubframe::VideoSubframe(VideoFrame& frame, uint32_t x, uint32_t y, uint32_t width, uint32_t height) : m_frame(frame), m_x(x), m_y(y), m_width(width), m_height(height)
	{
	}

	void VideoSubframe::Clear()
	{
		uint32_t pixelSize = m_frame.GetPixelSize();
		uint32_t dataWidth = m_width * pixelSize;
		uint32_t stride = m_frame.GetStride();
		uint32_t offset = m_x * pixelSize;

		for (uint32_t y = 0; y < m_height; y++)
			memset(m_frame.GetData() + offset +  y * stride, 0, dataWidth);
	}

	uint32_t VideoSubframe::CountLines() const
	{
		return m_height;
	}

	uint32_t VideoSubframe::GetStride() const
	{
		return m_frame.GetStride();
	}

	uint32_t VideoSubframe::GetDataWidth() const
	{
		return m_width * m_frame.GetPixelSize();
	}

	uint8_t* VideoSubframe::GetLineData(uint32_t lineNumber)
	{
		if (lineNumber > m_height)
			throw exception(strobj() << "VideoSubframe::GetLineData() - Invalid line number " << lineNumber << " because this subframe only has " << m_height << " lines");

		uint32_t pixelSize = m_frame.GetPixelSize();
		uint32_t stride = m_frame.GetStride();
		uint32_t offset = lineNumber * stride + m_x * pixelSize;

		return m_frame.GetData() + offset;
	}
}