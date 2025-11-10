//
// Created by Lieven Schokker on 10/11/2025.
//

#ifndef GAMEENGINE_SDLIMAGE_H
#define GAMEENGINE_SDLIMAGE_H
#include "../Asset.h"
#include <SDL_image.h>
#include <SDL.h>

class SDLImage : public Asset
{
public:
	SDLImage();
	~SDLImage();

	SDLImage(const SDLImage &) = delete;

	SDLImage &operator=(const SDLImage &) = delete;



	bool Load(const std::string &filepath) override;

	bool UnLoad() override;

	bool IsLoaded() const override;

private:
	SDL_Surface *surface;
};


#endif //GAMEENGINE_SDLIMAGE_H
