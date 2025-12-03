#pragma once

#include "Audio/Handles.h"

#include <string>

/**
 * @interface IAudioBackend
 * @brief Backend-agnostic interface for audio playback.
 *
 * Implemented by concrete backends (e.g., SDL, FMOD, OpenAL) to provide
 * loading, playback, and control over music and sound effects.
 */
class IAudioBackend
{
   public:
	virtual ~IAudioBackend() = default;

	/**
	 * @brief Initializes the backend audio system.
	 * @return True on success.
	 */
	virtual bool initialize() = 0;

	/**
	 * @brief Loads a sound effect.
	 * @param path File path.
	 * @return Sound handle, or -1 on failure.
	 */
	virtual SoundHandle loadSound(const std::string& path) = 0;

	/**
	 * @brief Loads a music track.
	 * @param path File path.
	 * @return Music handle, or -1 on failure.
	 */
	virtual MusicHandle loadMusic(const std::string& path) = 0;

	/**
	 * @brief Unloads a sound effect.
	 */
	virtual void unloadSound(SoundHandle handle) = 0;

	/**
	 * @brief Unloads a music track.
	 */
	virtual void unloadMusic(MusicHandle handle) = 0;

	/**
	 * @brief Plays a sound on a given channel.
	 * @param handle Sound handle.
	 * @param channel Backend channel index.
	 * @param loops Loop count (-1 infinite).
	 */
	virtual void playSound(SoundHandle handle, int channel, int loops) = 0;

	/**
	 * @brief Starts playing music.
	 * @param handle Music handle.
	 * @param loops Loop count.
	 * @return True on success.
	 */
	virtual bool playMusic(MusicHandle handle, int loops) = 0;

	/**
	 * Pauses music playback.
	 */
	virtual void pauseMusic() = 0;

	/**
	 * Resumes paused music.
	 */
	virtual void resumeMusic() = 0;

	/**
	 * @brief Stops playback on a sound channel.
	 */
	virtual void stopChannel(int channel) = 0;

	/**
	 * @brief Stops all music playback.
	 */
	virtual void stopMusic() = 0;

	/**
	 * @brief Sets music volume (0.0–1.0).
	 */
	virtual void setMusicVolume(float volume) = 0;

	/**
	 * @brief Sets stereo panning for a channel.
	 * @param left  Left volume (0.0–1.0)
	 * @param right Right volume (0.0–1.0)
	 */
	virtual void setChannelPanning(int channel, float left, float right) = 0;

	/**
	 * @brief Asks backend for any free playback channel.
	 * @return Channel index, or -1 if none available.
	 */
	virtual int reserveFreeChannel() = 0;
};
