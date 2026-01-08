#pragma once

#include <string>

#include "Animation/SpritesheetDefinition.h"

class AssetManager;
class SpriteComponent;

/**
 * @class SpritesheetLoader
 * @brief Utility class for loading spritesheet images and configuring
 * sprite components.
 *
 * This class provides static methods to load spritesheet assets and configure
 * sprite components. It cannot be instantiated.
 */
class SpritesheetLoader
{
   public:
	/**
	 * @brief Loads a spritesheet image and configures a sprite component.
	 *
	 * Loads the image asset, creates a spritesheet definition, and sets it on
	 * the sprite component. The asset manager must already exist and be
	 * initialized.
	 *
	 * @param assetManager Pointer to the AssetManager to use for loading
	 * @param sprite Pointer to the SpriteComponent to configure
	 * @param path File path to the spritesheet image
	 * @param rows Number of rows in the spritesheet grid
	 * @param cols Number of columns in the spritesheet grid
	 * @param frameWidth Width of each frame in pixels
	 * @param frameHeight Height of each frame in pixels
	 * @return true if spritesheet loaded successfully, false otherwise
	 */
	static bool loadSpritesheet(AssetManager* assetManager,
								SpriteComponent* sprite,
								const std::string& path, int rows, int cols,
								int frameWidth, int frameHeight);

    static bool loadSpritesheet(AssetManager* assetManager, SpriteComponent* sprite, const std::string& path, SpritesheetDefinition definition);

   private:
	// Prevent instantiation
	SpritesheetLoader() = delete;
};
