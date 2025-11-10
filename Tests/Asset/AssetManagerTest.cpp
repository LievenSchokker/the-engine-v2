#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>

#include "../src/Assets/AssetManager/AssetManager.h"
#include "../src/Assets/SDLAssetTypes/SDLImage.h"


class AssetManagerTest : public ::testing::Test {
protected:
	AssetManager* manager{};
	const char* testImagePath{};
	void SetUp() override {
		std::cout << "SDL_image initialized successfully" << std::endl;

		manager = new AssetManager();
		testImagePath = "/Users/sirbarry/Documents/GitHub/the-engine-repo/tests/Asset/Sprite/MissingTexture.jpg";

		std::cout << "SetUp complete!" << std::endl;
	}

	void TearDown() override {
		delete manager;
	}
};

// Test 1: Load a single image
TEST_F(AssetManagerTest, LoadSingleImage) {
	//Arrange
	manager->Add(testImagePath, std::make_unique<SDLImage>());

	//Act
	bool loaded = manager->Load(testImagePath);

	//Assert
	EXPECT_TRUE(loaded) << "Failed to load image: " << SDL_GetError();
}

// Test 2: Prevent duplicate loading
TEST_F(AssetManagerTest, PreventDuplicateLoad) {
	//Arrange
	manager->Add(testImagePath, std::make_unique<SDLImage>());
	manager->Load(testImagePath);

	// Act
	bool loadedAgain = manager->Load(testImagePath);

	// Assert
	EXPECT_TRUE(loadedAgain) << "Duplicate load should return true";
}



