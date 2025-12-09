#pragma once

#include "Assets/IImage.h"

#include <SDL.h>
#include <SDL_image.h>

class SDLImage: public IImage
{
   public:
	SDLImage();

	~SDLImage();

	SDLImage(const SDLImage&) = delete;

	SDLImage& operator=(const SDLImage&) = delete;

	bool load(const std::string& filepath) override;

	bool unLoad() override;

	bool isLoaded() const override;

	/**
	 * @brief Gets the width of the loaded image surface.
	 * @return Width in pixels, or 0 if not loaded
	 */
	int getWidth() const override;

	/**
	 * @brief Gets the height of the loaded image surface.
	 * @return Height in pixels, or 0 if not loaded
	 */
	int getHeight() const override;

	/**
	 * @brief Gets the SDL surface (for texture conversion in renderer).
	 *
	 * This is an implementation detail exposed only for SDLRenderer's internal
	 * use. Other code should use the IImage interface instead.
	 *
	 * @return Pointer to SDL_Surface, or nullptr if not loaded
	 */
	SDL_Surface* getSurface() const;

   private:
	SDL_Surface* surface;
};
