#include "Audio/SDL/AudioBackendSDL.h"

#include "SDL.h"
#include "SDL_mixer.h"

#include <algorithm>
#include <iostream>

AudioBackendSDL::~AudioBackendSDL()
{
	shutdown();
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

bool AudioBackendSDL::loadSound(const std::string& path)
{
	if ( sounds.contains(path) ) return true;

	Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
	if ( !chunk ) return false;

	sounds[path] = chunk;
	return true;
}

bool AudioBackendSDL::loadMusic(const std::string& path)
{
	if ( music.contains(path) ) return true;

	Mix_Music* mus = Mix_LoadMUS(path.c_str());
	if ( !mus )
	{
		std::cerr << "Failed to load music: " << path << " - " << Mix_GetError()
				  << std::endl;
		return false;
	}

	music[path] = mus;
	return true;
}

void AudioBackendSDL::unloadSound(const std::string& path)
{
	auto it = sounds.find(path);
	if ( it != sounds.end() )
	{
		Mix_FreeChunk(it->second);
		sounds.erase(it);
	}
}

void AudioBackendSDL::unloadMusic(const std::string& path)
{
	auto it = music.find(path);
	if ( it == music.end() ) return;

	Mix_FreeMusic(it->second);
	music.erase(it);
}

void AudioBackendSDL::playSound(const std::string& path, int channel, int loops)
{
	auto it = sounds.find(path);
	if ( it == sounds.end() ) return;

	Mix_PlayChannel(channel, it->second, loops);
}

void AudioBackendSDL::playMusic(const std::string& path, int loops)
{
	auto it = music.find(path);
	if ( it == music.end() ) return;

	Mix_PlayMusic(it->second, loops);
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

void AudioBackendSDL::shutdown()
{
	// Cleanup audio
	for ( auto& s : sounds ) Mix_FreeChunk(s.second);
	for ( auto& m : music ) Mix_FreeMusic(m.second);

	sounds.clear();
	music.clear();

	if ( initialized )
	{
		Mix_CloseAudio();
		Mix_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	initialized = false;
}