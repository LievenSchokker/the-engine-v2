///
/// Created by Lieven Schokker on 10/11/2025.
///


#include "Assets/SDLImage.h"
#include "Assets/AssetTypes.h"


#include <iostream>




SDLImage::SDLImage() : surface(nullptr)
{
	assetType = AssetType::Sprite;
}


SDLImage::~SDLImage()
{
	unLoad();
}


bool SDLImage::load(const std::string & filepath)
{
	unLoad();

	surface = IMG_Load(filepath.c_str());

	if(surface == nullptr)
	{
		std::cerr << "IMG_Load failed: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}


bool SDLImage::unLoad()
{
	if(surface != nullptr)
	{
		SDL_FreeSurface(surface);
		surface = nullptr;
	}

	return true;
}


bool SDLImage::isLoaded() const
{
	return surface	!= nullptr;
}
