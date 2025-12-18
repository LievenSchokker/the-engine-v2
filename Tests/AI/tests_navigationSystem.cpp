#include <gtest/gtest.h>
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/Pathfinding/AStarPathFinder.h"
#include "AI/Navigation/BoundingBox.h"

TEST(NavigationSystemTests, ComputePathWithNoSurfaceReturnsEmpty)
{
    NavigationSystem system(nullptr);
    AStarPathFinder pathfinder;

    PathResult result = system.computePath(pathfinder, {0, 0}, {1, 1});
    EXPECT_FALSE(result.isValid());
}

TEST(NavigationSystemTests, BakeWithNoSurfaceDoesNotCrash)
{
    NavigationSystem system(nullptr);
    EXPECT_NO_THROW(system.bake({}));
}

TEST(NavigationSystemTests, SetNavigationSurfaceStoresSurface)
{
    auto grid = std::make_unique<NavigationGrid>(5, 5, Vector2{1, 1});
    NavigationSystem system(std::move(grid));

    EXPECT_NE(system.getNavigationSurface(), nullptr);
}
