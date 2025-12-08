#pragma once

#include <string>

class AssetManager;
class SpriteComponent;

/**
 * @namespace SpritesheetLoader
 * @brief Utility functions for loading spritesheet images and configuring
 * sprite components.
 */
namespace SpritesheetLoader
{
/**
 * @brief Loads a spritesheet image and configures a sprite component.
 *
 * Loads the image asset, creates a spritesheet definition, and sets it on the
 * sprite component. The asset manager must already exist and be initialized.
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
bool loadSpritesheet(AssetManager* assetManager, SpriteComponent* sprite,
					 const std::string& path, int rows, int cols,
					 int frameWidth, int frameHeight);
}  // namespace SpritesheetLoader
