
#include <gtest/gtest.h>
#include "AI/Navigation/BoundingBox.h"

TEST(BoundingBoxTests, SizeIsCalculatedCorrectly)
{
    BoundingBox box({1, 2}, {4, 6});
    Vector2 size = box.size();

    EXPECT_FLOAT_EQ(size.x, 3);
    EXPECT_FLOAT_EQ(size.y, 4);
}

TEST(BoundingBoxTests, CenterIsCalculatedCorrectly)
{
    BoundingBox box({0, 0}, {10, 10});
    Vector2 center = box.center();

    EXPECT_FLOAT_EQ(center.x, 5);
    EXPECT_FLOAT_EQ(center.y, 5);
}

TEST(BoundingBoxTests, HandlesNegativeCoordinates)
{
    BoundingBox box({-5, -5}, {5, 5});
    Vector2 size = box.size();
    Vector2 center = box.center();

    EXPECT_FLOAT_EQ(size.x, 10);
    EXPECT_FLOAT_EQ(size.y, 10);
    EXPECT_FLOAT_EQ(center.x, 0);
    EXPECT_FLOAT_EQ(center.y, 0);
}