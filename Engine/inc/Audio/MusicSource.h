#pragma once

#include "AudioAssetManager.h"
#include "Component/Component.h"

#include <string>

class AudioManager;

/**
 * @class MusicSource
 * @brief Component responsible for loading and playing background music.
 *
 * MusicSource represents a single logical music player. It communicates with
 * the AudioManager to load music assets and control playback parameters.
 */
class MusicSource : public Component
{
public:
	MusicSource() = default;
	~MusicSource() = default;

	/**
	 * @brief Loads a music file through the AudioManager.
	 * @param path File path to the music asset.
	 * @return True if loading succeeded.
	 */
	bool loadMusic(const std::string& path);

	/**
	 * @brief Starts playback of the loaded music track.
	 */
	void play();

	/**
	 * @brief Stops the music playback.
	 */
	void stop();

	/**
	 * @brief Pauses the currently playing music.
	 */
	void pause();

	/**
	 * @brief Resumes paused music.
	 */
	void resume();

	/**
	 * @brief Sets the playback volume.
	 * @param volume Range: 0.0–1.0.
	 */
	void setVolume(float volume);

	/**
	 * @brief Enables or disables looping playback.
	 * @param shouldLoop True to loop, false to play once.
	 */
	void setLoop(bool shouldLoop);

	/// Path or tag of the loaded music track.
	std::string musicAssetTag;

	/// Reference to the AudioManager controlling playback.
	AudioManager* audioManager = nullptr;

private:
	bool playing = false;      ///< Whether the music is currently playing.
	bool loop = false;         ///< Should the music loop?
	float volume = 0.9f;       ///< Playback volume.
	MusicHandle handle = -1;   ///< Handle to the loaded music asset.
};
