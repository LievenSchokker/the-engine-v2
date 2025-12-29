#include "Assets/TilemapAsset.h"
#include "Component/GridComponent.h"
#include "Component/TilemapComponent.h"
#include "GameObject/GameObject.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace
{
    struct TempTilemapFile
    {
        std::filesystem::path path;

        explicit TempTilemapFile(const std::string& contents)
        {
            path = std::filesystem::temp_directory_path() /
                std::filesystem::path("grid_component_test.csv");
            std::ofstream file(path);
            file << contents;
            file.close();
        }

        ~TempTilemapFile()
        {
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
        }
    };
} // namespace

TEST(GridComponentTest, CustomWalkableTileIds)
{
    // Arrange
    TempTilemapFile temp("0,2\n3,1\n");
    TilemapAsset asset;
    ASSERT_TRUE(asset.load(temp.path.string()));

    GameObject go;
    auto* tilemap = go.addComponent<TilemapComponent>();
    tilemap->setTilemapAsset(&asset);

    auto* grid = go.addComponent<GridComponent>();
    grid->setTilemapComponent(tilemap);
    grid->setWalkableTileIds({0, 2});

    // Act
    const bool walkableZero = grid->isWalkable({0, 0});
    const bool walkableTwo = grid->isWalkable({1, 0});
    const bool blockedThree = grid->isWalkable({0, 1});

    // Assert
    EXPECT_TRUE(walkableZero);
    EXPECT_TRUE(walkableTwo);
    EXPECT_FALSE(blockedThree);
}

TEST(GridComponentTest, WalkableTileIdOverrides)
{
    // Arrange
    TempTilemapFile temp("1,0\n0,1\n");
    TilemapAsset asset;
    ASSERT_TRUE(asset.load(temp.path.string()));

    GameObject go;
    auto* tilemap = go.addComponent<TilemapComponent>();
    tilemap->setTilemapAsset(&asset);

    auto* grid = go.addComponent<GridComponent>();
    grid->setTilemapComponent(tilemap);

    // Act
    const bool defaultWalkable =
        grid->isWalkable({1, 0}); // Tile ID 0 should be walkable
    grid->addWalkableTileId(1);
    const bool customWalkable = grid->isWalkable({0, 0}); // Tile ID 1
    grid->clearWalkableTileIds();
    const bool clearedWalkable = grid->isWalkable({1, 0});

    // Assert
    EXPECT_TRUE(defaultWalkable);
    EXPECT_TRUE(customWalkable);
    EXPECT_FALSE(clearedWalkable);
}

TEST(GridComponentTest, RuntimeBlockerMask)
{
    // Arrange
    TempTilemapFile temp("0,0\n0,0\n");
    TilemapAsset asset;
    ASSERT_TRUE(asset.load(temp.path.string()));

    GameObject go;
    auto* tilemap = go.addComponent<TilemapComponent>();
    tilemap->setTilemapAsset(&asset);

    auto* grid = go.addComponent<GridComponent>();
    grid->setTilemapComponent(tilemap);

    const Vector2 targetCell{1.0, 0.0};

    // Act & Assert
    EXPECT_TRUE(grid->isWalkable(targetCell));
    EXPECT_FALSE(grid->isCellBlocked(targetCell));

    grid->blockCell(targetCell);
    EXPECT_TRUE(grid->isCellBlocked(targetCell));
    EXPECT_FALSE(grid->isWalkable(targetCell));

    grid->unblockCell(targetCell);
    EXPECT_FALSE(grid->isCellBlocked(targetCell));
    EXPECT_TRUE(grid->isWalkable(targetCell));

    grid->blockCell(targetCell);
    grid->clearBlockedCells();
    EXPECT_FALSE(grid->isCellBlocked(targetCell));
    EXPECT_TRUE(grid->isWalkable(targetCell));
}
