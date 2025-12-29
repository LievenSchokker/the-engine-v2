#include "Assets/SpritesheetLoader.h"

#include "Animation/SpritesheetDefinition.h"
#include "Assets/AssetManager.h"
#include "Assets/IImage.h"
#include "Assets/SDLImage.h"
#include "Component/SpriteComponent.h"

#include <iostream>

bool SpritesheetLoader::loadSpritesheet(AssetManager* assetManager,
										SpriteComponent* sprite,
										const std::string& path, int rows,
										int cols, int frameWidth,
										int frameHeight)
{
	if ( assetManager == nullptr )
	{
		std::cerr << "SpritesheetLoader: AssetManager is null\n";
		return false;
	}

	if ( sprite == nullptr )
	{
		std::cerr << "SpritesheetLoader: SpriteComponent is null\n";
		return false;
	}

	// Add and load the image asset
	if ( !assetManager->has(path) )
	{
		assetManager->add(path, std::make_unique<SDLImage>());
	}

	if ( !assetManager->load(path) )
	{
		std::cerr << "SpritesheetLoader: Failed to load spritesheet: " << path
				  << "\n";
		return false;
	}

	IImage* image = dynamic_cast<IImage*>(assetManager->get(path));
	if ( image == nullptr || !image->isLoaded() )
	{
		std::cerr << "SpritesheetLoader: Failed to get loaded image: " << path
				  << "\n";
		return false;
	}

	// Create spritesheet definition
	SpritesheetDefinition def{rows, cols, frameWidth, frameHeight};

	// Set sprite on component
	sprite->setSprite(image, def);

	return true;
}
