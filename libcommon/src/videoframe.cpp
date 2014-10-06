#include "libcommon/videoframe.h"
#include "libcommon/videoframepool.h"

#include <dzlib/dzexception.h>
#include <slog/slog.h>

#include <boost/thread/mutex.hpp>

namespace dz
{
	VideoFrame::VideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format) : m_width_cap(width), m_height_cap(height), m_format(format), m_width(width), m_height(height)
	{
		switch (m_format)
		{
			case VideoFrameFormat::RGB: m_pixelSize = 3; break;
			case VideoFrameFormat::RGBA: m_pixelSize = 4; break;
			default: throw exception(strobj() << "Unknown pixel format " << m_format << " alteast to GetPixelSize()");
		}

		m_pData.reset(new uint8_t[m_height * m_width * GetPixelSize()]);
	}

	VideoFrame::~VideoFrame()
	{
	}

	void VideoFrame::Clear()
	{
		memset(m_pData.get(), 0, m_height * m_width * GetPixelSize());
	}

	uint32_t VideoFrame::GetStride() const
	{
		return m_width * GetPixelSize();
	}

	uint8_t* VideoFrame::GetData() const
	{
		return m_pData.get();
	}

	bool VideoFrame::Resize(uint32_t new_width, uint32_t new_height)
	{
		if (new_width > m_width_cap || new_height > m_height_cap)
		{
			try
			{
				m_pData = std::unique_ptr<uint8_t>(new uint8_t[new_width * new_height * GetPixelSize()]);
				m_width_cap = new_width;
				m_height_cap = new_height;
			}
			catch (std::bad_alloc&)
			{
				return false;
			}
		}

		m_width = new_width;
		m_height = new_height;
		return true;
	}

	//=======================================

	//static 
	void VideoFrameHandle::VideoFrameDeleter(VideoFrame* frame)
	{
		VideoFramePool::GetInstance().FreeVideoFrame(frame);
	}

	VideoFrameHandle::VideoFrameHandle(VideoFrame* handledFrame) : std::unique_ptr<VideoFrame, void(*)(VideoFrame*)>(handledFrame, VideoFrameDeleter)
	{
	}

	VideoFrameHandle::VideoFrameHandle(VideoFrameHandle&& other) : std::unique_ptr<VideoFrame, void(*)(VideoFrame*)>(std::move(other))
	{
	}
}
