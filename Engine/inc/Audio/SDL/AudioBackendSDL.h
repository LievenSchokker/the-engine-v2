#pragma once

#include "Audio/IAudioBackend.h"
#include "Audio/Handles.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class AudioBackendSDL : public IAudioBackend
{
public:
	AudioBackendSDL() = default;
	~AudioBackendSDL() override;

	bool initialize() override;

	SoundHandle loadSound(const std::string& path) override;
	MusicHandle loadMusic(const std::string& path) override;

	void unloadSound(SoundHandle handle) override;
	void unloadMusic(MusicHandle handle) override;

	void playSound(SoundHandle handle, int channel, int loops) override;
	bool playMusic(MusicHandle handle, int loops) override;
	void pauseMusic() override;
	void resumeMusic() override;

	void stopChannel(int channel) override;
	void stopMusic() override;

	void setMusicVolume(float volume) override;
	void setChannelPanning(int channel, float left, float right) override;

   private:
	// Internal storage
	std::unordered_map<SoundHandle, Mix_Chunk*> soundMap;
	std::unordered_map<MusicHandle, Mix_Music*> musicMap;

	SoundHandle nextSoundHandle = 1;
	MusicHandle nextMusicHandle = 1;

	bool initialized = false;
};
