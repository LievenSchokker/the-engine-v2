#pragma once

#include "Asset.h"

/**
 * @class IImage
 * @brief Abstract interface for image assets
 *
 * This interface provides an abstraction layer for image loading and querying,
 * allowing the engine to work with different image implementations without
 * coupling to specific rendering backends.
 */
class IImage: public Asset
{
   public:
	virtual ~IImage() = default;

	/**
	 * @brief Gets the width of the loaded image.
	 * @return Width in pixels, or 0 if not loaded
	 */
	virtual int getWidth() const = 0;

	/**
	 * @brief Gets the height of the loaded image.
	 * @return Height in pixels, or 0 if not loaded
	 */
	virtual int getHeight() const = 0;
};
