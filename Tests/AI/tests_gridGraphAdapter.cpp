#include <gtest/gtest.h>
#include "AI/Navigation/NavigationGridGraphAdapter.h"
#include "AI/Navigation/NavigationGrid.h"

class NavigationGridGraphAdapterTest : public ::testing::Test
{
    protected:
        NavigationGrid grid{5, 5, {1, 1}};
        NavigationGridGraphAdapter adapter{grid};
};

TEST_F(NavigationGridGraphAdapterTest, ValidCellIsValid)
{
    EXPECT_TRUE(adapter.isValid({2, 2}));
}

TEST_F(NavigationGridGraphAdapterTest, BlockedCellIsInvalid)
{
    grid.setWalkable({2, 2}, false);
    EXPECT_FALSE(adapter.isValid({2, 2}));
}

TEST_F(NavigationGridGraphAdapterTest, DiagonalCostIsHigherThanStraight)
{
    float straight = adapter.getCost({1, 1}, {2, 1});
    float diagonal = adapter.getCost({1, 1}, {2, 2});

    EXPECT_GT(diagonal, straight);
}