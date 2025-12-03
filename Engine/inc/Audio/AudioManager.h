#pragma once

#include "Audio/Handles.h"
#include "AudioAssetManager.h"

#include <memory>
#include <vector>

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
    /**
     * @brief Constructs an empty AudioManager.
     */
    AudioManager() = default;

    /**
     * @brief Destructor.
     */
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
     * @brief Registers a positional audio source.
     * @param source The AudioSource to register.
     */
    void registerAudioSource(AudioSource* source);

    /**
     * @brief Unregisters a positional audio source.
     * @param source The AudioSource to remove.
     */
    void unregisterAudioSource(AudioSource* source);

    /**
     * @brief Loads a music track through the backend.
     * @param path File path to the music file.
     * @return True if loading succeeded.
     */
    bool loadMusic(const std::string& path) const;

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
     * @brief Sets the position listener for 3D audio.
     * @param listener The AudioListener reference.
     * @return True if assigned.
     */
    bool setAudioListener(AudioListener* listener);

    /**
     * @return The current audio listener.
     */
    AudioListener* getAudioListener() const;

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

private:
    /// Pointer to the low-level backend.
    std::unique_ptr<IAudioBackend> backend;

    /// Manager responsible for loading/unloading audio assets.
    std::unique_ptr<AudioAssetManager> assetManager;

    /// List of positional audio sources.
    std::vector<AudioSource*> audioSources;

    /// Active music-playing component.
    MusicSource* musicSource = nullptr;

    /// Global audio listener for 3D audio support.
    AudioListener* listener = nullptr;

    /// Global volume multipliers.
    float soundVolume = 1.0f;
    float musicVolume = 1.0f;
};