#include <libgrabber/src/Grabber.h>
#include <libgrabber/src/IDesktopTools.h>

#include <gtest/gtest.h>

class GrabberTest : public testing::Test
{
	protected:
		void SetUp () {
			m_grabber.reset(dz::IGrabber::create(dz::GrabberType::Default));
			m_desktopTools.reset(dz::IDesktopTools::CreateDesktopTools());
		}

		void TearDown () {
			m_grabber = nullptr;
		}

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
static void fillWithColor(dz::Buffer * buf, int32_t color)
{
	for (int y = 0; y < buf->height; y++)
	{
		for (int x = 0; x < buf->width; x++)
		{
			int32_t * pos = (int32_t*) (buf->data + (y * buf->rowLength) + x * 4);
			*pos = color;
		}
	}
}

/// Checks if a block is still in a specific color
static bool isAllInColor (const dz::Buffer * buf, int32_t color) {
	for (int y = 0; y < buf->height; y++) {
		for (int x = 0; x < buf->width; x++) {
			const int32_t * pos = (const int32_t*) (buf->data + (y * buf->rowLength) + x * 4);
			if (*pos != color) return false;
		}
	}
	return true;
}

static int32_t colorWhite = 0xffffffff;


TEST_F (GrabberTest, CheckTestFunctions)
{
	dz::Buffer buf (640, 480);
	fillWithColor (&buf, colorWhite);
	ASSERT_TRUE (isAllInColor (&buf, colorWhite)) << "Test functions do not work!";
	buf.data [640 * 4 * 117 + 318] = 0; // coord 318,117, first byte
	ASSERT_FALSE (isAllInColor (&buf, colorWhite)) << "Cannot detect small change";
}

TEST_F (GrabberTest, GrabEverything)
{
	dz::Rect all = m_desktopTools->GetCombinedScreenResolution();
	dz::Buffer buf (all.w, all.h);
	fillWithColor (&buf, colorWhite);
	m_grabber->grab(all, &buf);
	EXPECT_TRUE (!isAllInColor (&buf, colorWhite)) << "Grabbing should change at least one pixel!";
}

TEST_F (GrabberTest, GrabPart) {
	dz::Rect part (100, 100, 50, 50);
	dz::Buffer buf (part.w, part.h);
	fillWithColor (&buf, colorWhite);
	m_grabber->grab(part, &buf);
	EXPECT_TRUE (!isAllInColor (&buf, colorWhite)) << "Should change at least one pixel";
}

/// Grabs each screen for it self.
TEST_F (GrabberTest, SingleScreens)
{
	uint32_t screenCount = m_desktopTools->GetScreenCount();
	for (uint32_t i = 0; i < screenCount; i++)
	{
		dz::Rect screenRect = m_desktopTools->GetScreenResolution(i);
		dz::Buffer buf (screenRect.w, screenRect.h);
		m_grabber->grab(screenRect, &buf);
	}
}

/// Grabs areas usually not on a display
TEST_F (GrabberTest, BigExtends1)
{
	dz::Rect big (-1000, -1000, 10000, 10000);
	dz::Buffer buf (big.w, big.h);
	m_grabber->grab(big, &buf);
}

/// Grabs all screens and tests the color result (covers best multiple screens)
TEST_F (GrabberTest, GrabAllScreens)
{
	unsigned int color = 0x80808080;
	uint32_t screenCount = m_desktopTools->GetScreenCount();

	for (uint32_t i = 0; i < screenCount; i++)
	{
		dz::Rect screenRect = m_desktopTools->GetScreenResolution(i);
		dz::Buffer buffer(screenRect.w, screenRect.h);
		fillWithColor(&buffer, color);
		m_grabber->grab(screenRect, &buffer);
		EXPECT_TRUE(!isAllInColor(&buffer, color)) << "Each screen should contain the same color";
	}
}

TEST_F (GrabberTest, GrabAllScreensWithMouseCursor)
{
	unsigned int color = 0x55555555;
	uint32_t screenCount = m_desktopTools->GetScreenCount();
	m_grabber->setEnableGrabCursor(true);

	for (uint32_t i = 0; i < screenCount; i++)
	{
		dz::Rect screenRect = m_desktopTools->GetScreenResolution(i);
		dz::Buffer buffer(screenRect.w, screenRect.h);
		fillWithColor(&buffer, color);
		m_grabber->grab(screenRect, &buffer);
	}
}

TEST_F (GrabberTest, GrabAmongTwoScreens)
{
	unsigned int color = 0xffaabbcc;
	dz::Rect all = m_desktopTools->GetCombinedScreenResolution();
	dz::Buffer buf (all.w, all.h);
	fillWithColor (&buf, color);

	dz::Rect captureRect(1200, 200, 1024, 768);
	m_grabber->grab(captureRect, &buf);
}
