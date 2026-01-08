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
struct SpritesheetAnimationClip : ISerializable
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



    void serialize(WriteArchive& archive) const
    {
        archive.process(name);

        auto frameCount = static_cast<uint32_t>(frames.size());
        archive.process(frameCount);
        for (int frame : frames)
        {
            int32_t f = frame;
            archive.process(f);
        }

        float dur = frameDuration;
        archive.process(dur);

        uint8_t loopFlag = loop ? 1 : 0;
        archive.process(loopFlag);
    }

    void deserialize(ReadArchive& archive)
    {
        archive.process(name);

        uint32_t frameCount;
        archive.process(frameCount);
        frames.resize(frameCount);
        for (uint32_t i = 0; i < frameCount; ++i)
        {
            int32_t f;
            archive.process(f);
            frames[i] = static_cast<int>(f);
        }

        archive.process(frameDuration);

        uint8_t loopFlag;
        archive.process(loopFlag);
        loop = (loopFlag != 0);
    }
};
