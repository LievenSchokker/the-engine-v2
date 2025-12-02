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
	void stop();
	void pause();
	void resume();
	void setVolume(float volume);
	void setLoop(bool shouldLoop);

	std::string musicAssetTag;
	AudioManager* audioManager = nullptr;

private:
	bool playing = false;
	bool loop = false;
	float volume = 0.9f;
	MusicHandle handle = -1;
};
