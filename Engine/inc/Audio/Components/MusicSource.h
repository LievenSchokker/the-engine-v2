#pragma once

#include "Component/BaseComponentTypes/Component.h"

#include <string>

#include "Networking/Serialization/RegistrationBase.h"

class AudioManager;

/**
 * @class MusicSource
 * @brief Component responsible for loading and playing background music.
 *
 * MusicSource represents a single logical music player. It communicates with
 * the AudioManager to load music assets and control playback parameters.
 */
class MusicSource: public Component, RegistrationBase<MusicSource>
{
   public:
    MusicSource() = default;
	MusicSource(AudioManager* audioManagerPtr);
	~MusicSource();

    static constexpr const char* name()
    {
        return "MusicSource";
    }
    const char* getName() const override { return name(); }
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

	float getCurrentVolume() const;


   private:
	/// Path or tag of the loaded music track.
	std::string path;

	/// Reference to the AudioManager controlling playback.
	AudioManager* audioManager = nullptr;

	///< Whether the music is currently playing.
	bool playing = false;

	///< Should the music loop?
	bool loop = false;

	///< Playback volume.
	float volume = 0.9f;
};
