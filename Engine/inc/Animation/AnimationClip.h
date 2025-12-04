#pragma once

#include "Animation/AnimationTrack.h"

#include <string>
#include <vector>

/**
 * @class AnimationClip
 * @brief Container for multiple AnimationTracks that play together.
 *
 * An AnimationClip represents a complete animation sequence composed of
 * multiple tracks that can animate different properties simultaneously.
 */
class AnimationClip
{
   public:
	/**
	 * @brief Constructs an AnimationClip.
	 *
	 * @param name Name of the clip
	 * @param loop Whether the clip should loop when finished
	 */
	AnimationClip(const std::string& name, bool loop = false);

	/**
	 * @brief Gets the name of this clip.
	 * @return The clip name
	 */
	const std::string& getName() const;

	/**
	 * @brief Sets the name of this clip.
	 * @param name The new name
	 */
	void setName(const std::string& name);

	/**
	 * @brief Gets the total length (duration) of this clip.
	 *
	 * The length is the duration of the longest track.
	 *
	 * @return Duration in seconds
	 */
	float getLength() const;

	/**
	 * @brief Checks if this clip should loop.
	 * @return true if looping, false otherwise
	 */
	bool isLooping() const;

	/**
	 * @brief Sets whether this clip should loop.
	 * @param loop true to loop, false otherwise
	 */
	void setLoop(bool loop);

	/**
	 * @brief Adds a track to this clip.
	 *
	 * Adding a track may update the clip's length if the new track is longer.
	 *
	 * @param track The AnimationTrack to add
	 */
	void addTrack(const AnimationTrack& track);

	/**
	 * @brief Gets all tracks in this clip.
	 * @return Const reference to the vector of tracks
	 */
	const std::vector<AnimationTrack>& getTracks() const;

	/**
	 * @brief Gets all tracks in this clip (non-const).
	 * @return Reference to the vector of tracks
	 */
	std::vector<AnimationTrack>& getTracks();

   private:
	std::string name;
	float length;
	bool loop;
	std::vector<AnimationTrack> tracks;

	/**
	 * @brief Recalculates the clip length based on track durations.
	 */
	void updateLength();
};
