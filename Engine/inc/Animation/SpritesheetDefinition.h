#pragma once

/**
 * @struct SpritesheetDefinition
 * @brief Describes the grid layout of a spritesheet for frame-based animation.
 *
 * Defines how frames are arranged in a spritesheet image using a grid system.
 * Frames are read left-to-right, top-to-bottom (row-major order).
 */
struct SpritesheetDefinition
{
	/**
	 * @brief Number of rows in the spritesheet grid.
	 */
	int rows;

	/**
	 * @brief Number of columns in the spritesheet grid.
	 */
	int columns;

	/**
	 * @brief Width of each frame in pixels.
	 */
	int frameWidth;

	/**
	 * @brief Height of each frame in pixels.
	 */
	int frameHeight;

	/**
	 * @brief Gets the total number of frames in the spritesheet.
	 * @return Total frames (rows * columns)
	 */
	int getTotalFrames() const
	{
		return rows * columns;
	}
};
