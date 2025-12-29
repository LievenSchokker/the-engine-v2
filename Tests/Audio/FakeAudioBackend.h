#pragma once
#include "Audio/IAudioBackend.h"

struct FakeAudioBackend: public IAudioBackend
{
	bool initialized = false;

	// Logged calls
	std::string lastPlaySound_path = "";
	int lastPlaySound_channel = -1;
	int lastPlaySound_loops = -1;

	std::string lastPlayMusic_path = "";
	int lastPlayMusic_loops = -1;

	int lastStoppedChannel = -1;

	float lastMusicVolume = -1.0f;
	float lastPanLeft = -1.0f;
	float lastPanRight = -1.0f;

	int freeChannel = 1;

	// --- Backend API ---
	bool initialize() override
	{
		initialized = true;
		return true;
	}

	bool loadSound(const std::string& path) override
	{
		// Pretend "load succeeded"
		return true;
	}

	bool loadMusic(const std::string& path) override
	{
		// Pretend "load succeeded"
		return true;
	}

	void unloadSound(const std::string& path) override
	{
	}

	void unloadMusic(const std::string& path) override
	{
	}

	void playSound(const std::string& path, int channel, int loops) override
	{
		lastPlaySound_path = path;
		lastPlaySound_channel = channel;
		lastPlaySound_loops = loops;
	}

	void playMusic(const std::string& path, int loops) override
	{
		lastPlayMusic_path = path;
		lastPlayMusic_loops = loops;
	}

	void pauseMusic() override
	{
	}

	void resumeMusic() override
	{
	}

	void stopMusic() override
	{
	}

	void stopChannel(int channel) override
	{
		lastStoppedChannel = channel;
	}

	void setMusicVolume(float volume) override
	{
		lastMusicVolume = volume;
	}

	void setChannelPanning(int channel, float left, float right) override
	{
		lastPanLeft = left;
		lastPanRight = right;
	}

	int reserveFreeChannel() override
	{
		return freeChannel;
	}

	void shutdown() override
	{
	}
};