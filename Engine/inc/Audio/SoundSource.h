#pragma once

#include "Component/Component.h"
#include "Audio/Handles.h"

#include <string>

class AudioManager;

/**
 * @class SoundSource
 * @brief Component responsible for loading and playing sound effects.
 *
 * SoundSource supports one-shot playback (explosions, UI sounds) and
 * optionally looping. Each SoundSource may play multiple times (Mix channels).
 */
class SoundSource : public Component
{
public:
	SoundSource() = default;
	~SoundSource() = default;

	/**
	 * @brief Loads a sound effect through the AudioManager.
	 * @param path File path to the sound asset.
	 * @return True if loading succeeded.
	 */
	bool loadSound(const std::string& path);

	/**
	 * @brief Plays the sound once or looped.
	 * @param loops How many times to loop (-1 = infinite).
	 */
	void play(int loops = 0);

	/**
	 * @brief Stops playback on this source's channel.
	 */
	void stop();

	/**
	 * @brief Sets left-right stereo panning.
	 * @param left  Volume for left speaker (0.0–1.0)
	 * @param right Volume for right speaker (0.0–1.0)
	 */
	void setPanning(float left, float right);

	/// Tag/path of the loaded sound track.
	std::string soundAssetTag;

	/// Pointer to the audio manager.
	AudioManager* audioManager = nullptr;

private:
	SoundHandle handle = -1;
	int channel = -1;
};
