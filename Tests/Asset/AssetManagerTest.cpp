#include <filesystem>
#include <iostream>
#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>

#include "Assets/SDLAssetTypes/SDLImage.h"
#include "Assets/AssetManager.h"


class AssetManagerTest : public ::testing::Test {
protected:
	AssetManager* manager{};
	std::string testImagePath{};  

	void SetUp() override {
		std::cout << "SDL_image initialized successfully" << std::endl;

		manager = new AssetManager();

		std::filesystem::path testFilePath = __FILE__;
		std::filesystem::path testDir = testFilePath.parent_path();
		testImagePath = (testDir / "Sprite" / "MissingTexture.jpg").string();
	}

	void TearDown() override {
		delete manager;
	}
};

// Test 1: Load a single image
TEST_F(AssetManagerTest, LoadSingleImage) {
	//Arrange
	manager->add(testImagePath, std::make_unique<SDLImage>());

	//Act
	bool loaded = manager->load(testImagePath);

	//Assert
	EXPECT_TRUE(loaded) << "Failed to load image: " << SDL_GetError();
}

// Test 2: Prevent duplicate loading
TEST_F(AssetManagerTest, PreventDuplicateLoad) {
	//Arrange
	manager->add(testImagePath, std::make_unique<SDLImage>());
	manager->load(testImagePath);

	// Act
	bool loadedAgain = manager->load(testImagePath);

	// Assert
	EXPECT_TRUE(loadedAgain) << "Duplicate load should return true";
}

// Test 3: Prevent duplicate addition of same asset
TEST_F(AssetManagerTest, PreventDuplicateAdd) {
	//Arrange & Act
	manager->add(testImagePath, std::make_unique<SDLImage>());
	Asset* firstAsset = manager->get(testImagePath);

	// Try to add duplicate
	manager->add(testImagePath, std::make_unique<SDLImage>());
	Asset* secondAsset = manager->get(testImagePath);

	// Assert - Should be the same asset (duplicate add was prevented)
	EXPECT_EQ(firstAsset, secondAsset) << "Duplicate add should be ignored, same asset pointer expected";
	EXPECT_TRUE(manager->has(testImagePath)) << "Asset should still exist";
}

