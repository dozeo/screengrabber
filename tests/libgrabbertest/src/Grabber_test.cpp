#include <libgrabber/src/igrabber.h>
#include <libgrabber/src/IDesktopTools.h>

#include <gtest/gtest.h>

#include <libcommon/videoframepool.h>

using dz::VideoFrameHandle;
using dz::VideoFramePool;
using dz::VideoFrame;

class GrabberTest : public testing::Test
{
	protected:
		void SetUp()
		{
			m_grabber.reset(dz::IGrabber::CreateGrabber(m_options));
			m_desktopTools.reset(dz::IDesktopTools::CreateDesktopTools());
		}

		void TearDown()
		{
			m_grabber = nullptr;
		}

		VideoFramePool m_framePool;
		GrabberOptions m_options;
		std::unique_ptr<dz::IGrabber> m_grabber;
		std::unique_ptr<dz::IDesktopTools> m_desktopTools;
};

TEST_F (GrabberTest, Initialize) {
	// is empty
}

TEST_F (GrabberTest, FindAtLeastOneMonitor)
{
	int screenCount = m_desktopTools->GetScreenCount();
	ASSERT_TRUE (screenCount > 0);
}

/// Fills a buffer in a specific color
static void fillWithColor(VideoFrameHandle& frame, uint32_t color)
{
	if (frame->GetVideoFrameFormat() != dz::VideoFrameFormat::RGBA)
		throw std::exception("Wrong video frame format - expected RGBA");

	for (uint32_t y = 0; y < frame->GetHeight(); y++)
	{
		uint32_t* pData = reinterpret_cast<uint32_t*>(frame->GetData() + (y * frame->GetStride()));

		for (uint32_t x = 0; x < frame->GetWidth(); x++)
			pData[x] = color;
	}
}

/// Checks if a block is still in a specific color
static bool IsFrameContainingOnlyColor(dz::VideoFrameHandle& frame, uint32_t color)
{
	for (uint32_t y = 0; y < frame->GetHeight(); y++)
	{
		uint32_t* pData = reinterpret_cast<uint32_t*>(frame->GetData() + (y * frame->GetStride()));

		for (uint32_t x = 0; x < frame->GetWidth(); x++)
			if (pData[x] != color)
				return false;
	}

	return true;
}

#define RGBA(r, g, b, a) (((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b))
static uint32_t colorWhite = RGBA(255, 255, 255, 255);

TEST_F (GrabberTest, CheckTestFunctions)
{
	auto frame(VideoFramePool::GetInstance().AllocVideoFrame(640, 480, dz::VideoFrameFormat::RGBA));

	fillWithColor(frame, colorWhite);

	ASSERT_TRUE(IsFrameContainingOnlyColor(frame, colorWhite)) << "Test functions do not work!";

	uint32_t x = 318, y = 117;
	frame->GetData()[y * frame->GetStride() + x * frame->GetPixelSize()] = 0;

	ASSERT_FALSE(IsFrameContainingOnlyColor(frame, colorWhite)) << "Cannot detect small change";
}

TEST_F (GrabberTest, GrabEverything)
{
	dz::Rect all = m_desktopTools->GetCombinedScreenResolution();
	m_grabber->SetCaptureRect(all);

	auto frameHandle(m_grabber->GrabVideoFrame());
	const uint32_t blackColor = RGBA(0, 0, 0, 0);

	EXPECT_FALSE(IsFrameContainingOnlyColor(frameHandle, blackColor)) << "Grabbing should change at least one pixel!";
}

TEST_F (GrabberTest, GrabPart)
{
	dz::Rect part(100, 100, 50, 50);
	m_grabber->SetCaptureRect(part);
	auto frameHandle(m_grabber->GrabVideoFrame());
	//dz::Buffer buf (part.w, part.h);
	const uint32_t blackColor = RGBA(0, 0, 0, 0);
	//m_grabber->grab(part, &buf);
	EXPECT_FALSE(IsFrameContainingOnlyColor(frameHandle, blackColor)) << "Should change at least one pixel";
}

/// Grabs each screen for it self.
TEST_F (GrabberTest, SingleScreens)
{
	uint32_t screenCount = m_desktopTools->GetScreenCount();
	for (uint32_t i = 0; i < screenCount; i++)
	{
		dz::Rect screenRect = m_desktopTools->GetScreenResolution(i);
		
		m_grabber->SetCaptureRect(screenRect);
		auto frame(m_grabber->GrabVideoFrame());

		EXPECT_TRUE(frame->GetWidth() == screenRect.GetWidth()) << "Invalid width on frame";
		EXPECT_TRUE(frame->GetHeight() == screenRect.GetHeight()) << "Invalid height on frame";
	}
}

/// Grabs areas usually not on a display
TEST_F (GrabberTest, BigExtends1)
{
	dz::Rect big(-1000, -1000, 5000, 5000);

	m_grabber->SetCaptureRect(big);
	auto frame(m_grabber->GrabVideoFrame());

	EXPECT_TRUE(frame != nullptr) << "Run out of memory or could not allocate any more video frames";
}

TEST_F (GrabberTest, GrabAllScreensWithMouseCursor)
{
	unsigned int color = 0x55555555;
	uint32_t screenCount = m_desktopTools->GetScreenCount();
	m_grabber->setEnableGrabCursor(true);

	for (uint32_t i = 0; i < screenCount; i++)
	{
		dz::Rect screenRect = m_desktopTools->GetScreenResolution(i);
		m_grabber->SetCaptureRect(screenRect);
		auto frame(m_grabber->GrabVideoFrame());
	}
}

TEST_F (GrabberTest, GrabAmongTwoScreens)
{
	const uint32_t color = RGBA(0xaa, 0xbb, 0xcc, 0xff);
	dz::Rect all = m_desktopTools->GetCombinedScreenResolution();

	dz::Rect captureRect(1200, 200, 1024, 768);
	m_grabber->SetCaptureRect(captureRect);
	auto frame(m_grabber->GrabVideoFrame());
}

TEST_F (GrabberTest, GrabPartiallyOutside)
{
	const uint32_t color = RGBA(0xaa, 0xbb, 0xcc, 0xff);
	dz::Rect all = m_desktopTools->GetCombinedScreenResolution();

	all.x += all.width / 2;

	m_grabber->SetCaptureRect(all);
	auto frame(m_grabber->GrabVideoFrame());
}
