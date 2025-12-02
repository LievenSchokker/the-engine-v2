#pragma once

#include "Audio/Handles.h"
#include "IAudioBackend.h"

#include <map>
#include <string>

class AudioAssetManager
{
   public:
	explicit AudioAssetManager(IAudioBackend* backend) : backend(backend)
	{
	}

	SoundHandle loadSound(const std::string& path);
	MusicHandle loadMusic(const std::string& path);

	void releaseSound(SoundHandle handle);
	void releaseMusic(MusicHandle handle);

	void unloadUnused();
	void clear();

   private:
	IAudioBackend* backend;

	// std::map<std::string, MusicAsset> music;
};