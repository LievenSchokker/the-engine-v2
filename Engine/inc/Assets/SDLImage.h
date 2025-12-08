///
/// Created by Lieven Schokker on 10/11/2025.
///

#pragma once


#include "Asset.h"
#include <SDL_image.h>
#include <SDL.h>


class SDLImage : public Asset
{
public:
	SDLImage();

	~SDLImage();

	SDLImage(const SDLImage &) = delete;

	SDLImage &operator=(const SDLImage &) = delete;


	bool load(const std::string & filepath) override;

	bool unLoad() override;

	bool isLoaded() const override;

private:
	SDL_Surface *surface;
};
