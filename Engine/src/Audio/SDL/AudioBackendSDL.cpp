#include "Audio/SDL/AudioBackendSDL.h"

#include "Audio/Handles.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include <algorithm>
#include <iostream>

AudioBackendSDL::~AudioBackendSDL()
{
	// Cleanup audio
	for ( auto& s : soundMap ) Mix_FreeChunk(s.second);
	for ( auto& m : musicMap ) Mix_FreeMusic(m.second);

	soundMap.clear();
	musicMap.clear();

	if ( initialized )
	{
		Mix_CloseAudio();
		Mix_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}

bool AudioBackendSDL::initialize()
{
	if ( SDL_InitSubSystem(SDL_INIT_AUDIO) < 0 )
	{
		std::cerr << "SDL audio init error: " << SDL_GetError() << std::endl;
		return false;
	}

	int flags = MIX_INIT_OGG;
	int initted = Mix_Init(flags);
	if ( (initted & flags) != flags )
	{
		std::cerr << "SDL_mixer init error: " << Mix_GetError() << std::endl;
		// return false;
	}

	if ( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 )
	{
		std::cerr << "SDL_mixer open audio error: " << Mix_GetError()
				  << std::endl;
		return false;
	}

	Mix_AllocateChannels(32);

	initialized = true;
	return true;
}

SoundHandle AudioBackendSDL::loadSound(const std::string& path)
{
	// 1. Check cache first
	auto cached = soundCache.find(path);
	if ( cached != soundCache.end() ) return cached->second;

	// 2. Else load from disk
	Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
	if ( !chunk )
	{
		std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError()
				  << std::endl;
		return -1;
	}

	// 3. Assign new handle and enter into maps
	SoundHandle handle = nextSoundHandle++;
	soundMap[handle] = chunk;
	soundCache[path] = handle;

	return handle;
}

MusicHandle AudioBackendSDL::loadMusic(const std::string& path)
{
	// 1. Check cache
	auto cached = musicCache.find(path);
	if ( cached != musicCache.end() ) return cached->second;

	// 2. Load fresh asset
	Mix_Music* music = Mix_LoadMUS(path.c_str());
	if ( !music )
	{
		std::cerr << "Failed to load music: " << path << " - " << Mix_GetError()
				  << std::endl;
		return -1;
	}

	// 3. Store handle
	MusicHandle handle = nextMusicHandle++;
	musicMap[handle] = music;
	musicCache[path] = handle;

	return handle;
}

void AudioBackendSDL::unloadSound(SoundHandle handle)
{
	auto it = soundMap.find(handle);
	if ( it != soundMap.end() )
	{
		Mix_FreeChunk(it->second);
		soundMap.erase(it);
	}
}

void AudioBackendSDL::unloadMusic(MusicHandle handle)
{
	auto it = musicMap.find(handle);
	if ( it != musicMap.end() )
	{
		Mix_FreeMusic(it->second);
		musicMap.erase(it);
	}
}

void AudioBackendSDL::playSound(SoundHandle handle, int channel, int loops)
{
	auto it = soundMap.find(handle);
	if ( it == soundMap.end() ) return;

	Mix_PlayChannel(channel, it->second, loops);
}

bool AudioBackendSDL::playMusic(MusicHandle handle, int loops)
{
	auto it = musicMap.find(handle);
	if ( it == musicMap.end() ) return false;

	return Mix_PlayMusic(it->second, loops) == 0;
}

void AudioBackendSDL::pauseMusic()
{
	Mix_PauseMusic();
}

void AudioBackendSDL::resumeMusic()
{
	Mix_ResumeMusic();
}

void AudioBackendSDL::stopChannel(int channel)
{
	Mix_HaltChannel(channel);
}

void AudioBackendSDL::stopMusic()
{
	Mix_HaltMusic();
}

void AudioBackendSDL::setMusicVolume(float volume)
{
	const int vol = static_cast<int>(volume * MIX_MAX_VOLUME);
	Mix_VolumeMusic(vol);
}

void AudioBackendSDL::setChannelPanning(int channel, float left, float right)
{
	const float leftFloat = std::clamp(left, 0.0f, 1.0f);
	const float rightFloat = std::clamp(right, 0.0f, 1.0f);

	const Uint8 leftByte = static_cast<Uint8>(leftFloat * 255);
	const Uint8 rightByte = static_cast<Uint8>(rightFloat * 255);

	Mix_SetPanning(channel, leftByte, rightByte);
}

int AudioBackendSDL::reserveFreeChannel()
{
	return Mix_GroupAvailable(-1);
}