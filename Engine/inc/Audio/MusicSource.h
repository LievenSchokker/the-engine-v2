#pragma once

#include "AudioAssetManager.h"
#include "Component/Component.h"

#include <string>

class AudioManager;

class MusicSource: public Component
{
   public:
	MusicSource() = default;
	~MusicSource() = default;

	bool loadMusic(const std::string& path);
	void play();
	void stop() const;
	void pause();
	void resume();
	void setVolume(float volume);

	// Fields (could be made private + getters/setters)
	std::string musicAssetTag;
	AudioManager* audioManager = nullptr;
	// std::unique_ptr<AudioManager> audioManager = nullptr;
	AudioAssetManager* audioAssetManager = nullptr;

	float volume = 0.9f;
	bool loop = false;
	bool playOnAwake = false;

private:
	MusicHandle handle = -1;
};
