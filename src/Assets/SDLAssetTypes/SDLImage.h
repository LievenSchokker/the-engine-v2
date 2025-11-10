//
// Created by Lieven Schokker on 10/11/2025.
//

#ifndef GAMEENGINE_SDLIMAGE_H
#define GAMEENGINE_SDLIMAGE_H
#include "Assets/Asset.h"
#include <SDL_image.h>
#include <SDL.h>

class SDLImage : public Asset
{
public:
	SDLImage();

	SDLImage(const SDLImage &) = delete;

	SDLImage &operator=(const SDLImage &) = delete;

	virtual ~SDLImage() = delete;

	virtual bool Load(const std::string &filepath) = 0;

	virtual bool unLoad() = 0;

	virtual bool isLoaded() const = 0;

private:
	SDL_Surface *surface;
};


#endif //GAMEENGINE_SDLIMAGE_H
