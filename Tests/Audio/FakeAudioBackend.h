#pragma once
#include "Audio/IAudioBackend.h"

struct FakeAudioBackend: public IAudioBackend
{
	bool initialized = false;

	// Logged calls
	int lastPlaySound_channel = -1;
	int lastPlaySound_handle = -1;
	int lastPlaySound_loops = -1;

	int lastPlayMusic_handle = -1;
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

	SoundHandle loadSound(const std::string&) override
	{
		return 10;	// arbitrary handle
	}

	MusicHandle loadMusic(const std::string&) override
	{
		return 5;  // arbitrary handle
	}

	void unloadSound(SoundHandle) override
	{
	}

	void unloadMusic(MusicHandle) override
	{
	}

	void playSound(SoundHandle handle, int channel, int loops) override
	{
		lastPlaySound_handle = handle;
		lastPlaySound_channel = channel;
		lastPlaySound_loops = loops;
	}

	void playMusic(MusicHandle handle, int loops) override
	{
		lastPlayMusic_handle = handle;
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
		return;
	}
};
