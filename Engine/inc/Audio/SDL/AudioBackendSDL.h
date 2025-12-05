#pragma once

#include "Audio/Handles.h"
#include "Audio/IAudioBackend.h"

#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

/**
 * @class AudioBackendSDL
 * @brief SDL_mixer–based implementation of the IAudioBackend interface.
 *
 * This backend handles low-level audio loading and playback using SDL2 and
 * SDL_mixer. It manages internal storage for loaded sounds and music and
 * exposes an API that the AudioManager can interact with.
 */
class AudioBackendSDL: public IAudioBackend
{
   public:
	AudioBackendSDL() = default;

	/**
	 * @brief Destructor.
	 *
	 * Frees all loaded audio assets and shuts down SDL_mixer.
	 */
	~AudioBackendSDL() override;

	/**
	 * @brief Initializes SDL audio and SDL_mixer.
	 * @return True if initialization succeeded, false otherwise.
	 */
	bool initialize() override;

	/**
	 * @brief Loads a sound effect into memory.
	 * @param path Path to a WAV file.
	 * @return SoundHandle for the loaded sound, or -1 on failure.
	 */
	SoundHandle loadSound(const std::string& path) override;

	/**
	 * @brief Loads streamed music into memory.
	 * @param path Path to a MUS/OGG/MP3 file.
	 * @return MusicHandle for the loaded music, or -1 on failure.
	 */
	MusicHandle loadMusic(const std::string& path) override;

	/**
	 * @brief Unloads a previously loaded sound.
	 * @param handle The SoundHandle to unload.
	 */
	void unloadSound(SoundHandle handle) override;

	/**
	 * @brief Unloads a previously loaded music track.
	 * @param handle The MusicHandle to unload.
	 */
	void unloadMusic(MusicHandle handle) override;

	/**
	 * @brief Plays a sound effect.
	 * @param handle Sound to play.
	 * @param channel Mixer channel index.
	 * @param loops Number of loops (-1 = infinite).
	 */
	void playSound(SoundHandle handle, int channel, int loops) override;

	/**
	 * @brief Plays a music track.
	 * @param handle Music handle.
	 * @param loops Loop count (-1 = infinite).
	 * @return True on success, false on failure.
	 */
	bool playMusic(MusicHandle handle, int loops) override;

	/**
	 * @brief Pauses currently playing music.
	 */
	void pauseMusic() override;

	/**
	 * @brief Resumes a paused music track.
	 */
	void resumeMusic() override;

	/**
	 * @brief Stops playback on a specific sound channel.
	 * @param channel SDL_mixer channel index.
	 */
	void stopChannel(int channel) override;

	/**
	 * @brief Stops music playback.
	 */
	void stopMusic() override;

	/**
	 * @brief Sets the music volume.
	 * @param volume Range: 0.0–1.0.
	 */
	void setMusicVolume(float volume) override;

	/**
	 * @brief Applies stereo panning to a sound channel.
	 * @param channel Channel to affect.
	 * @param left Left speaker volume (0–1).
	 * @param right Right speaker volume (0–1).
	 */
	void setChannelPanning(int channel, float left, float right) override;

	int reserveFreeChannel() override;

   private:
	/// Map of sound handles to Mix_Chunk objects.
	std::unordered_map<SoundHandle, Mix_Chunk*> soundMap;

	/// Map of path and sound handle, to implement caching
	std::unordered_map<std::string, SoundHandle> soundCache;

	/// Map of music handles to Mix_Music objects.
	std::unordered_map<MusicHandle, Mix_Music*> musicMap;

	/// Map of path and music handle, to implement caching
	std::unordered_map<std::string, MusicHandle> musicCache;

	/// Next available sound handle.
	SoundHandle nextSoundHandle = 1;

	/// Next available music handle.
	MusicHandle nextMusicHandle = 1;

	/// Whether the backend successfully initialized.
	bool initialized = false;
};
