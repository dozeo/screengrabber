#include "grabber_null.h"
#include <assert.h>
#include <string.h>

#include <libcommon/videoframepool.h>
#include <libcommon/videosubframe.h>

namespace dz
{
	NullGrabber::NullGrabber () {

	}

	NullGrabber::~NullGrabber() {

	}

	Rect NullGrabber::screenResolution(int screen) const
	{
		if (screen == 0) return Rect (0,0, 1280, 800);
		if (screen == 1) return Rect (1280, 0, 1920, 1080);
		return Rect ();
	}

	VideoFrameHandle NullGrabber::GrabVideoFrame()
	{
		Rect captureRect = Rect(640, 480);

		// Qt defines them this way: 0xAARRGGBB;
		// This is internally BB, GG, RR, AA (little endian)
		// in OSX: kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little
		int32_t blue = 0x000000ff;
		Rect first = screenResolution(0);
		Rect second = screenResolution(1);

		Rect cut;

		uint32_t width = captureRect.GetWidth();
		uint32_t height = captureRect.GetHeight();

		auto videoFrame(VideoFramePool::GetInstance().AllocVideoFrame(width, height, VideoFrameFormat::RGBA));
		if (videoFrame == nullptr)
			return videoFrame;

		videoFrame->Clear();

		for (uint32_t screenId = 0; screenId < 2; screenId++)
		{
			Rect screenRect = screenResolution(screenId);
		
			if (captureRect.intersects(screenRect, &cut))
			{
				uint32_t relX = static_cast<uint32_t>(cut.x - captureRect.x);
				uint32_t relY = static_cast<uint32_t>(cut.y - captureRect.y);
			
				VideoSubframe subframe(*videoFrame, relX, relY, cut.width, cut.height);
				uint32_t pixelSize = subframe.GetPixelSize();
				uint32_t frameWidth = subframe.GetWidth();
			
				for (uint32_t i = 0; i < subframe.GetHeight(); i++)
				{
					auto pLine = subframe.GetLineData(i);
					for (uint32_t x = 0; x < frameWidth; x++)
					{
						uint32_t* pPixel = reinterpret_cast<uint32_t*>(pLine + x * pixelSize);
						*pPixel = blue;
					}
				}
			}
		}

		return videoFrame;
	}
}
