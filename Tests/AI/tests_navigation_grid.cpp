//
// Created by samle on 14/12/2025.
//

#include <gtest/gtest.h>

#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationGrid.h"

class NavigationGridTest : public ::testing::Test
{
    protected:
        std::unique_ptr<NavigationSystem> navSystem;
        NavigationGrid* grid;

        void SetUp() override
        {
            std::unique_ptr<NavigationGrid> navGrid = std::make_unique<NavigationGrid>(10, 10, Vector2{5.0f,5.0f});
            grid = navGrid.get();
            navSystem = std::make_unique<NavigationSystem>(std::move(navGrid));
        }

        void TearDown() override
        {
            grid = nullptr;
        }
};


TEST_F(NavigationGridTest, NavigationGridConstructor)
{
    ASSERT_NE(grid, nullptr);

    // Check dimensions
    EXPECT_EQ(grid->getWidth(), 10);
    EXPECT_EQ(grid->getHeight(), 10);

    // Check cell size
    Vector2 cellSize = grid->getCellSize();
    EXPECT_FLOAT_EQ(cellSize.x, 5.0f);
    EXPECT_FLOAT_EQ(cellSize.y, 5.0f);

    // Check that all cells are initially set and are walkable.
    for (int y = 0; y < grid->getHeight(); ++y)
    {
        for (int x = 0; x < grid->getWidth(); ++x)
        {
            EXPECT_TRUE(grid->isWalkable({static_cast<float>(x), static_cast<float>(y)}));
        }
    }
}

TEST_F(NavigationGridTest, WorldToCellClampsToGrid)
{
    Vector2 cell = grid->worldToCellPosition({-10, -10});
    EXPECT_EQ(cell.x, 0);
    EXPECT_EQ(cell.y, 0);
}

TEST_F(NavigationGridTest, SetWalkableMarksCellUnwalkable)
{
    EXPECT_TRUE(grid->isWalkable({5,5}));

    grid->setWalkable({5, 5}, false);

    EXPECT_FALSE(grid->isWalkable({5, 5}));
}