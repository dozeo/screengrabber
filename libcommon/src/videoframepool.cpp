#include "libcommon/videoframepool.h"

#include <dzlib/dzexception.h>

namespace dz
{
	VideoFramePool* VideoFramePool::m_instance = nullptr;

	VideoFramePool::VideoFramePool(uint32_t maxFrames) : m_curFrames(0), m_maxFrames(maxFrames)
	{
		if (m_instance != nullptr)
			throw exception(strstream() << "Another Video frame pool already exists!");

		m_instance = this;
	}

	VideoFramePool::~VideoFramePool()
	{
		assert (m_instance == this);
	}

	//static
	VideoFramePool& VideoFramePool::GetInstance()
	{
		if (m_instance == nullptr)
			throw exception(strstream() << "No VideoFramePool instance found");

		return *m_instance;
	}

	VideoFrameHandle VideoFramePool::AllocVideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format)
	{
		while (true)
		{
			VideoFrame* curFrame = GetNextFreeVideoFrame();

			if (curFrame != nullptr)
			{
				if (curFrame->GetWidth() != width ||
					curFrame->GetHeight() != height ||
					curFrame->GetVideoFrameFormat() != format)
				{
					delete curFrame;
					curFrame = nullptr;
				}

				if (curFrame != nullptr)
				{
					VideoFrameHandle handle(curFrame);
					return std::move(handle);
				}
			}
			else
			{
				VideoFrameHandle handle(NewVideoFrame(width, height, format));
				return std::move(handle);
			}
		}
	}

	void VideoFramePool::FreeVideoFrame(VideoFrame* videoFrame)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_freeVideoFrames.push_back(videoFrame);
	}

	VideoFrame* VideoFramePool::GetNextFreeVideoFrame()
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (m_freeVideoFrames.size() == 0)
			return nullptr;

		VideoFrame* videoFrame = m_freeVideoFrames.back();
		m_freeVideoFrames.pop_back();

		return videoFrame;
	}

	VideoFrame* VideoFramePool::NewVideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (m_curFrames < m_maxFrames)
		{
			m_curFrames++;
			return new VideoFrame(width, height, format);
		}

		return nullptr;
	}
}

// EOF
