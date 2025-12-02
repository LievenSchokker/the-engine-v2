#pragma once

#include "Audio/Handles.h"
#include "AudioAssetManager.h"

#include <memory>
#include <vector>

class IAudioBackend;
class AudioSource;
class MusicSource;
class AudioListener;

class AudioManager
{
public:
	AudioManager() = default;
	~AudioManager() = default;

	bool initialize(std::unique_ptr<IAudioBackend> backendPtr);
	void shutdown();

	void registerAudioSource(AudioSource* source);
	void unregisterAudioSource(AudioSource* source);

	bool loadMusic(const std::string& path) const;
	bool setMusicSource(MusicSource* source);
	void unsetMusicSource(MusicSource* source);

	bool setAudioListener(AudioListener* listener);
	AudioListener* getAudioListener() const;

	void setSoundVolume(float volume);
	void setMusicVolume(float volume);

	void playMusic(MusicHandle handle, bool loop) const;
	void pauseMusic() const;
	void stopMusic() const;
	void resumeMusic() const;

   private:
	std::unique_ptr<IAudioBackend> backend;
	std::unique_ptr<AudioAssetManager> assetManager;
	std::vector<AudioSource*> audioSources;
	MusicSource* musicSource = nullptr;
	AudioListener* listener = nullptr;

	float soundVolume = 1.0f;
	float musicVolume = 1.0f;
};
