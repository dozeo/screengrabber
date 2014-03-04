#include "include/libcommon/videoframe.h"

#include <dzlib/dzexception.h>

#include <boost/thread/mutex.hpp>

namespace dz
{
	VideoFrame::VideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format) : m_width(width), m_height(height), m_format(format)
	{
		m_pData.reset(new uint8_t[height * m_width * GetPixelSize()]);
	}

	VideoFrame::~VideoFrame()
	{
	}

	uint32_t VideoFrame::GetPixelSize() const
	{
		switch (m_format)
		{
			case VideoFrameFormat::RGB: return 3;
			case VideoFrameFormat::RGBA: return 4;
			default: throw exception(strstream() << "Unknown pixel format " << m_format << " alteast to GetPixelSize()");
		}
	}

	uint32_t VideoFrame::GetStride() const
	{
		return m_width * GetPixelSize();
	}

	uint8_t* VideoFrame::GetData() const
	{
		return m_pData.get();
	}
}
