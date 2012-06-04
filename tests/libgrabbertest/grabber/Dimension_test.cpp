#include <gtest/gtest.h>
#include <grabber/Dimension.h>

using dz::Dimension2;

class DimensionTest : public ::testing::Test {

};

TEST_F(DimensionTest, equalsSameDimension)
{
	Dimension2 actual(4, 6);
	Dimension2 expected(4, 6);

	EXPECT_TRUE(expected == actual);
}
