#include <gtest/gtest.h>
#include <libcommon/dzrect.h>

using dz::Rect;

class RectTest : public ::testing::Test {
public:
};

TEST_F(RectTest, leftRectLiesNextToRight)
{
	Rect actual(0, 0, 10, 20);
	Rect other(30, 0, 15, 20);

	EXPECT_FALSE(actual.intersects(other));
	EXPECT_FALSE(other.intersects(actual));
}

TEST_F(RectTest, smallerRectintersectsBigger)
{
	Rect bigger(10, 10, 100, 80);
	Rect smaller(20, 40, 50, 30);

	EXPECT_TRUE(smaller.intersects(bigger));
	EXPECT_TRUE(bigger.intersects(smaller));
}

TEST_F(RectTest, smallerRectCompletelyInsideOther)
{
	Rect smaller(50, 40, 25, 30);
	Rect bigger(40, 30, 50, 60);

	EXPECT_TRUE(smaller.intersects(bigger));
	EXPECT_TRUE(bigger.intersects(smaller));
}

TEST_F(RectTest, captureRectIntersectsScreenRect)
{
	Rect screen(0, 0, 1680, 1050);
	Rect capture(1000, 0, 640, 480);

	EXPECT_TRUE(capture.intersects(screen));
	EXPECT_TRUE(screen.intersects(capture));
}

TEST_F(RectTest, rectOutsideOtherRectVertically)
{
	Rect top(10, 10, 50, 20);
	Rect bottom(15, 100, 40, 40);

	EXPECT_FALSE(top.intersects(bottom));
	EXPECT_FALSE(bottom.intersects(top));
}

TEST_F(RectTest, intersectinRect)
{
	Rect left(20, 10, 60, 40);
	Rect right(50, 10, 60, 40);

	Rect intersect;
	EXPECT_TRUE(left.intersects(right, &intersect));

	EXPECT_EQ(50, intersect.x);
	EXPECT_EQ(10, intersect.y);
	EXPECT_EQ(30, intersect.w);
	EXPECT_EQ(40, intersect.h);
}

TEST_F (RectTest, intersect2) 
{
    Rect monitor (0, 0, 1440, 900);
    Rect piece (100, 100, 50, 50);
    Rect intersect;
    EXPECT_TRUE (monitor.intersects (piece, &intersect));
    EXPECT_EQ (intersect, piece) << "Hey, the piece is exactly inside the monitor";
}

TEST_F (RectTest, addTwoScreensToBoundingRect)
{
	Rect screen0 (0, 100, 1680, 1050);
	Rect screen1 (1680, 0, 1920, 1280);

	Rect expected (0, 0, 1680 + 1920, 1280);
	Rect actual;
	actual.addToBoundingRect(screen0);
	actual.addToBoundingRect(screen1);

	EXPECT_EQ(expected, actual) << "Bounding rect must contain both screens!";
}

TEST_F (RectTest, cornerBoundingBox)
{
	Rect a;
	Rect b (10, 20, 30, 40);
	Rect c = b;
	Rect d;
	a.addToBoundingRect(b);
	EXPECT_EQ (a, b) << "Empty rects must be ignored as bounding boxes";

	c.addToBoundingRect(d);
	EXPECT_EQ (b, b) << "Empty rects must be ignored as bounding boxes";
}

TEST_F (RectTest, overlappingBound) {
	Rect a (10, 20, 100, 200);
	Rect b (20, 30, 100, 200);
	Rect r;
	r.addToBoundingRect(a);
	r.addToBoundingRect(b);
	Rect exp (10, 20, 110, 210);
	EXPECT_EQ (r, exp) << "Bounding Addition does not work";
}

TEST_F (RectTest, containedBound) {
	Rect a (10, 10, 100, 200);
	Rect b (15, 15, 10, 20);
	Rect exp = a;
	a.addToBoundingRect(b);
	EXPECT_EQ (a, exp) << "Adding a box inside should not increase the BB";
}

TEST_F (RectTest, bugAddScreenAsBoundingRectToAnotherScreen)
{
	Rect screen0 (1920, 30, 1680, 1050);
	Rect screen1 (   0,  0, 1920, 1080);

	Rect actual;
	actual.addToBoundingRect (screen0);
	actual.addToBoundingRect (screen1);

	Rect expected (0, 0, 3600, 1080);

	EXPECT_EQ(expected, actual) << "Bounding rect must contain both screens!";
}


TEST_F (RectTest, bottomRightBoundaryWithOffsetPos)
{
	Rect screen (1920, 30, 1680, 1050);

	EXPECT_EQ (1680, screen.w);
	EXPECT_EQ (1050, screen.h);
	EXPECT_EQ (1920, screen.left());
	EXPECT_EQ (  30, screen.top());
	EXPECT_EQ (1920, screen.x);
	EXPECT_EQ (  30, screen.y);

	EXPECT_EQ (3600, screen.right());
	EXPECT_EQ (3600, screen.x + screen.w);
	EXPECT_EQ (1080, screen.bottom());
	EXPECT_EQ (1080, screen.y + screen.h);
}

TEST_F (RectTest, containsTest) {
	Rect screen0 (-50, -60, 150, 170);
	EXPECT_TRUE (screen0.contains (-50,-60));
	EXPECT_FALSE (screen0.contains (-50,-61));
	EXPECT_FALSE (screen0.contains (-51,-60));
	EXPECT_FALSE (screen0.contains (100, 110));
	EXPECT_TRUE  (screen0.contains (99, 109));
	EXPECT_FALSE (screen0.contains (100, 109));
	EXPECT_FALSE (screen0.contains (99, 110));
	EXPECT_TRUE (screen0.contains (0,0));
}
