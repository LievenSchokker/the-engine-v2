#pragma once

/**
 * @struct Rect
 * @brief Rectangle structure for source regions in images
 *
 * This structure provides an abstraction for rectangular regions, replacing
 * backend-specific types like SDL_Rect. This allows components to work
 * independently of the rendering backend.
 */
struct Rect
{
	int x = 0;	///< X coordinate of the top-left corner
	int y = 0;	///< Y coordinate of the top-left corner
	int w = 0;	///< Width of the rectangle
	int h = 0;	///< Height of the rectangle

	/**
	 * @brief Default constructor - creates an empty rectangle
	 */
	Rect() = default;

	/**
	 * @brief Constructor with parameters
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param w Width
	 * @param h Height
	 */
	Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h)
	{
	}

	/**
	 * @brief Checks if the rectangle is empty (zero width or height)
	 * @return true if empty, false otherwise
	 */
	bool isEmpty() const
	{
		return w <= 0 || h <= 0;
	}
};
