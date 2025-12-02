#pragma once

#include "Audio/Handles.h"

#include <cstddef>
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

	bool initialize(IAudioBackend* backend);
	void shutdown();

	void registerAudioSource(AudioSource* source);
	void unregisterAudioSource(AudioSource* source);

	bool setMusicSource(MusicSource* source);
	void unsetMusicSource(MusicSource* source);

	bool setAudioListener(AudioListener* listener);
	AudioListener* getAudioListener() const;

	void setSoundVolume(float volume);
	void setMusicVolume(float volume);

	void playMusic(MusicHandle handle, bool loop);
	void stopMusic();

private:
	IAudioBackend* backend = nullptr;

	std::vector<AudioSource*> audioSources;
	MusicSource* musicSource = nullptr;
	AudioListener* listener = nullptr;

	float soundVolume = 1.0f;
	float musicVolume = 1.0f;
};
