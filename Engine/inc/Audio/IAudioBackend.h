#pragma once

#include "Audio/Handles.h"

#include <string>


class IAudioBackend
{
   public:
	virtual ~IAudioBackend() = default;

	virtual bool initialize() = 0;

	virtual SoundHandle loadSound(const std::string& path) = 0;
	virtual MusicHandle loadMusic(const std::string& path) = 0;

	virtual void unloadSound(SoundHandle handle) = 0;
	virtual void unloadMusic(MusicHandle handle) = 0;

	virtual void playSound(SoundHandle handle, int channel, int loops) = 0;
	virtual bool playMusic(MusicHandle handle, int loops) = 0;
	virtual void pauseMusic() = 0;
	virtual void resumeMusic() = 0;

	virtual void stopChannel(int channel) = 0;
	virtual void stopMusic() = 0;

	virtual void setMusicVolume(float volume) = 0;
	virtual void setChannelPanning(int channel, float left, float right) = 0;
};