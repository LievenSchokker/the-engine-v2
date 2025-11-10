//
// Created by Lieven Schokker on 10/11/2025.
//

#include "SDLImage.h"

#include <iostream>

SDLImage::SDLImage()
{
	m_assetType = AssetType::Sprite;
}

bool SDLImage::Load(const std::string &filepath)
{
	unLoad();

	surface = IMG_Load(filepath.c_str());

	if (surface == nullptr)
	{
		std::cerr << "IMG_Load failed: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool SDLImage::unLoad()
{
	if (surface != nullptr)
	{
		SDL_FreeSurface(surface);
		surface = nullptr;
	}
	return true;
}

bool SDLImage::isLoaded() const { return surface != nullptr; }
