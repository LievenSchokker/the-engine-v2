#pragma once

#include <string>
#include <vector>

/**
 * @struct SpritesheetAnimationClip
 * @brief High-level animation definition for spritesheet animations.
 *
 * Game programmers use this struct to define animations in a simple,
 * Unity-like way without exposing internal engine details.
 *
 * Example usage:
 * @code
 * SpritesheetAnimationClip walk;
 * walk.frames = {0, 1, 2, 3};
 * walk.frameDuration = 0.1f;
 * walk.loop = true;
 * walk.name = "walk";
 * @endcode
 */
struct SpritesheetAnimationClip
{
	/**
	 * @brief Frame sequence to animate through.
	 *
	 * For simple animations: {0, 1, 2, 3}
	 * For ping-pong: {0, 1, 2, 1}
	 * For single frame: {0}
	 */
	std::vector<int> frames;

	/**
	 * @brief Duration of each frame in seconds.
	 *
	 * Total animation duration = frames.size() * frameDuration
	 */
	float frameDuration = 0.1f;

	/**
	 * @brief Whether the animation should loop.
	 */
	bool loop = true;

	/**
	 * @brief Name of the animation clip.
	 *
	 * Used to identify and play the animation by name.
	 */
	std::string name;
};
