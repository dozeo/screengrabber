#include "libcommon/videoframe.h"
#include "libcommon/videoframepool.h"

#include <dzlib/dzexception.h>

#include <boost/thread/mutex.hpp>

namespace dz
{
	VideoFrame::VideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format) : m_width(width), m_height(height), m_format(format)
	{
		switch (m_format)
		{
			case VideoFrameFormat::RGB: m_pixelSize = 3; break;
			case VideoFrameFormat::RGBA: m_pixelSize = 4; break;
			default: throw exception(strstream() << "Unknown pixel format " << m_format << " alteast to GetPixelSize()");
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
