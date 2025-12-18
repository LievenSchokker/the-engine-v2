#include <gtest/gtest.h>
#include "AI/Navigation/Pathfinding/PathResult.h"


TEST(PathResultTests, EmptyPathIsInvalid)
{
    PathResult result = PathResult::empty();

    EXPECT_FALSE(result.isValid());
    EXPECT_EQ(result.getPathSize(), 0);
    EXPECT_FLOAT_EQ(result.getPathLength(), 0.0f);
}

TEST(PathResultTests, SinglePointPathIsInvalid)
{
    PathResult result({ {0, 0} });

    EXPECT_FALSE(result.isValid());
    EXPECT_EQ(result.getPathSize(), 0);
}

TEST(PathResultTests, TwoPointPathIsValid)
{
    PathResult result({ {0, 0}, {3, 4} });

    EXPECT_TRUE(result.isValid());
    EXPECT_EQ(result.getPathSize(), 2);
    EXPECT_FLOAT_EQ(result.getPathLength(), 5.0f);
}
