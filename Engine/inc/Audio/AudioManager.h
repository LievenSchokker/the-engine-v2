#pragma once

#include "Audio/Handles.h"

#include <memory>
#include <string>

class IAudioBackend;
class AudioSource;
class MusicSource;
class AudioListener;

/**
 * @class AudioManager
 * @brief High-level audio controller for all sound and music playback.
 *
 * The AudioManager owns:
 * - an audio backend (IAudioBackend)
 * - the AudioAssetManager
 * - all registered AudioSources
 *
 * It exposes a clean interface for loading and playing music and sounds,
 * delegating low-level work to the backend.
 */
class AudioManager
{
   public:
	AudioManager() = default;
	~AudioManager() = default;

	/**
	 * @brief Initializes the audio manager with a backend.
	 * @param backendPtr Unique pointer to a backend implementation.
	 * @return True if backend initialization succeeded.
	 */
	bool initialize(std::unique_ptr<IAudioBackend> backendPtr);

	/**
	 * @brief Shuts down the backend and clears all audio data.
	 */
	void shutdown();

	/**
	 * @brief Loads a music track through the backend.
	 * @param path File path to the music file.
	 * @return True if loading succeeded.
	 */
	MusicHandle  loadMusic(const std::string& path) const;

	/**
	 * @brief Assigns the active music source.
	 * @param source The MusicSource playing music.
	 * @return True if assigned.
	 */
	bool setMusicSource(MusicSource* source);

	/**
	 * @brief Removes the active music source.
	 * @param source Source requesting removal.
	 */
	void unsetMusicSource(MusicSource* source);

	/**
	 * @brief Sets the global sound volume.
	 * @param volume Range: 0.0–1.0.
	 */
	void setSoundVolume(float volume);

	/**
	 * @brief Sets the global music volume.
	 * @param volume Range: 0.0–1.0.
	 */
	void setMusicVolume(float volume);

	/**
	 * @brief Plays a music track by handle.
	 * @param handle Music handle.
	 * @param loop Whether the track should loop.
	 */
	void playMusic(MusicHandle handle, bool loop) const;

	/**
	 * @brief Pauses the music.
	 */
	void pauseMusic() const;

	/**
	 * @brief Stops music playback.
	 */
	void stopMusic() const;

	/**
	 * @brief Resumes paused music.
	 */
	void resumeMusic() const;

	/**
	 * @brief Loads a sound effect from disk.
	 * @param path Path to the audio file.
	 * @return Handle to the loaded sound, or -1 on failure.
	 */
	SoundHandle loadSound(const std::string& path) const;

	/**
	 * @brief Plays a loaded sound, with the given stereo on the channel
	 * @param handle Sound to play.
	 * @param loops Number of loops (0 = once, -1 = infinite).
	 * @param left  Left volume (0.0–1.0).
	 * @param right Right volume (0.0–1.0).
	 * @return Channel index used for playback, or -1 on failure.
	 */
	int playSound(SoundHandle handle, int loops, float left, float right) const;

	/**
	 * @brief Stops playback on a specific channel.
	 * @param channel Channel index to stop.
	 */
	void stopChannel(int channel) const;

	/**
	 * @brief Applies stereo panning to a channel, is reset after use.
	 * And is only meant to change the panning of a already running sound.
	 * @param channel Channel index.
	 * @param left  Left volume (0.0–1.0).
	 * @param right Right volume (0.0–1.0).
	 */
	void setChannelPanning(int channel, float left, float right) const;

   private:
	/// Pointer to the low-level backend.
	std::unique_ptr<IAudioBackend> backend;

	/// Active music-playing component.
	MusicSource* musicSource = nullptr;

	/// Global audio listener for 3D audio support.
	AudioListener* listener = nullptr;

	/// Global volume multipliers.
	float soundVolume = 1.0f;
	float musicVolume = 1.0f;
};