#pragma once

#include "libcommon/videoframe.h"
#include <cstdint>
#include <vector>
#include <boost/thread/mutex.hpp>

namespace dz
{
	class VideoFramePool
	{
		public:
			friend class VideoFrameHandle;

			VideoFramePool(uint32_t maxFrames = 5);
			~VideoFramePool();

			static VideoFramePool& GetInstance();

			VideoFrameHandle AllocVideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format = VideoFrameFormat::RGBA);

		protected:
			void FreeVideoFrame(VideoFrame* videoFrame);

			VideoFrame* GetNextFreeVideoFrame();
			
			VideoFrame* NewVideoFrame(uint32_t width, uint32_t height, VideoFrameFormat::Enum format);

		private:
			//VideoFramePool();
			VideoFramePool(const VideoFramePool&); 

			std::vector<VideoFrame*> m_freeVideoFrames;
			boost::mutex m_mutex;
			uint32_t m_curFrames, m_maxFrames;

			static VideoFramePool* m_instance;
	};
};