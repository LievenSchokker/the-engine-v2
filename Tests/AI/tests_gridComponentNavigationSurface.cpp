#include <gtest/gtest.h>

#include "AI/Navigation/BoundingBox.h"
#include "AI/Navigation/GridComponentGraphAdapter.h"
#include "AI/Navigation/GridComponentNavigationSurface.h"
#include "Assets/TilemapAsset.h"
#include "Component/GridComponent.h"
#include "Component/TilemapComponent.h"
#include "GameObject/GameObject.h"
#include "Component/Transform.h"
#include <filesystem>
#include <fstream>

namespace
{
struct TempTilemapFile
{
	std::filesystem::path path;

	explicit TempTilemapFile(const std::string& contents)
	{
		path = std::filesystem::temp_directory_path() /
		       std::filesystem::path("grid_component_nav_test.csv");
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

TEST(GridComponentNavigationSurfaceTest, AdapterUsesWalkabilityAndWeights)
{
	TempTilemapFile temp("0,1\n2,0\n");
	TilemapAsset asset;
	ASSERT_TRUE(asset.load(temp.path.string()));

	GameObject go;
	auto* tilemap = go.addComponent<TilemapComponent>();
	tilemap->setTilemapAsset(&asset);

	auto* grid = go.addComponent<GridComponent>();
	grid->setTilemapComponent(tilemap);
	grid->setWalkableTileIds({0, 2});
	grid->setTileWeight(2, 2.5);

	GridComponentGraphAdapter adapter(*grid);

	EXPECT_TRUE(adapter.isValid({0, 0}));
	EXPECT_FALSE(adapter.isValid({1, 0}));

	const float cost = adapter.getCost({0, 0}, {0, 1});
	EXPECT_GT(cost, 2.0f);
}

TEST(GridComponentNavigationSurfaceTest, SurfaceConvertsAndBakes)
{
	TempTilemapFile temp("0,0,0\n0,0,0\n0,0,0\n");
	TilemapAsset asset;
	ASSERT_TRUE(asset.load(temp.path.string()));

	GameObject go;
	go.getTransform()->setPosition({10.0f, 20.0f});

	auto* tilemap = go.addComponent<TilemapComponent>();
	tilemap->setTilemapAsset(&asset);
	tilemap->setTileSize({2.0f, 2.0f});

	auto* grid = go.addComponent<GridComponent>();
	grid->setTilemapComponent(tilemap);

	GridComponentNavigationSurface surface(*grid);

	Vector2 surfacePoint = surface.toSurfacePoint({13.0f, 25.0f});
	EXPECT_FLOAT_EQ(surfacePoint.x, 1.0f);
	EXPECT_FLOAT_EQ(surfacePoint.y, 2.0f);

	Vector2 worldPoint = surface.toWorldPoint({1.0f, 2.0f});
	EXPECT_FLOAT_EQ(worldPoint.x, 13.0f);
	EXPECT_FLOAT_EQ(worldPoint.y, 25.0f);

	EXPECT_TRUE(surface.isPointOnSurface({2.0f, 2.0f}));
	EXPECT_FALSE(surface.isPointOnSurface({3.0f, 3.0f}));

	BoundingBox obstacle({12.1f, 22.1f}, {13.9f, 23.9f});
	surface.bakeSurface({obstacle});

	EXPECT_TRUE(grid->isCellBlocked({1.0f, 1.0f}));
}
