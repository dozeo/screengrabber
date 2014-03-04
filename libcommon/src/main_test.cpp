#if 0

#include "libcommon/videoframe.h"
#include "libcommon/videoframepool.h"

#include <dzlib/dzexception.h>

#include <boost/thread/mutex.hpp>
#include <vector>
#include <cassert>

void main(int argc, char* argv[])
{
	dz::VideoFramePool framePool(5);

	std::vector<dz::VideoFrame*> frames;

	{
		uint32_t i = 10;
		while (i--)
		{
			frames.push_back(dz::VideoFramePool::GetInstance().AllocVideoFrame(640, 480, dz::VideoFrameFormat::RGBA));
		}
	}

	while (frames.size() > 0)
	{
		auto frame = frames.back();
		frames.pop_back();
		if (frame)
			framePool.FreeVideoFrame(frame);
	}

	{
		uint32_t i = 10;
		while (i--)
		{
			frames.push_back(dz::VideoFramePool::GetInstance().AllocVideoFrame(640, 480, dz::VideoFrameFormat::RGBA));
		}
	}
}

#endif // 0