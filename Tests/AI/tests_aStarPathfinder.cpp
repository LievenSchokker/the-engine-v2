#include <gtest/gtest.h>
#include "AI/Navigation/Pathfinding/AStarPathFinder.h"
#include "AI/Navigation/NavigationGrid.h"

class AStarPathFinderTest : public ::testing::Test
{
    protected:
        NavigationGrid grid{10, 10, {1, 1}};
        AStarPathFinder pathfinder;
};

TEST_F(AStarPathFinderTest, FindsPathInGrid)
{
    PathResult result = pathfinder.findPath(
        *grid.getPathfindingGraph(),
        {0, 0},
        {5, 5}
    );

    EXPECT_TRUE(result.isValid());
}

TEST_F(AStarPathFinderTest, ReturnsEmptyPathIfStartIsBlocked)
{
    grid.setWalkable({0, 0}, false);

    PathResult result = pathfinder.findPath(
        *grid.getPathfindingGraph(),
        {0, 0},
        {5, 5}
    );

    EXPECT_FALSE(result.isValid());
}

TEST_F(AStarPathFinderTest, PathAvoidsBlockedCell)
{
    grid.setWalkable({1, 0}, false);

    PathResult result = pathfinder.findPath(
        *grid.getPathfindingGraph(),
        {0, 0},
        {2, 0}
    );

    EXPECT_TRUE(result.isValid());

    for (const Vector2& point : result.getPath())
        EXPECT_NE(point, Vector2(1, 0));
}
