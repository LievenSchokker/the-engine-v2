#include "Audio/AudioManager.h"

#include "Audio/IAudioBackend.h"
#include "Audio/MusicSource.h"

bool AudioManager::initialize(std::unique_ptr<IAudioBackend> backendPtr)
{
	backend = std::move(backendPtr);
	if ( !backend->initialize() ) return false;
	assetManager = std::make_unique<AudioAssetManager>(backend.get());
	return true;
}

void AudioManager::shutdown()
{
	audioSources.clear();
	musicSource = nullptr;
	listener = nullptr;
	backend = nullptr;
}

void AudioManager::registerAudioSource(AudioSource* source)
{
	if ( !source ) return;

	audioSources.push_back(source);
}

void AudioManager::unregisterAudioSource(AudioSource* source)
{
	if ( !source ) return;

	for ( auto it = audioSources.begin(); it != audioSources.end(); ++it )
	{
		if ( *it == source )
		{
			audioSources.erase(it);
			return;
		}
	}
}


bool AudioManager::loadMusic(const std::string& path) const
{
	return backend->loadMusic(path);;
}


bool AudioManager::setMusicSource(MusicSource* source)
{
	if ( !source ) return false;

	musicSource = source;
	return true;
}

void AudioManager::unsetMusicSource(MusicSource* source)
{
	if ( musicSource == source ) musicSource = nullptr;
}

bool AudioManager::setAudioListener(AudioListener* newListener)
{
	if ( !newListener ) return false;

	listener = newListener;
	return true;
}

AudioListener* AudioManager::getAudioListener() const
{
	return listener;
}

void AudioManager::setSoundVolume(float volume)
{
	soundVolume = volume;

	// Optional: propagate to all audio sources
	// for (auto* src : audioSources)
	//     src->setVolume(soundVolume);
}

void AudioManager::setMusicVolume(float volume)
{
	musicVolume = volume;
	if ( !backend ) return;
	backend->setMusicVolume(volume);
}

void AudioManager::playMusic(MusicHandle handle, bool loop) const
{
	if ( !backend ) return;
	loop = loop ? -1 : 0;
	backend->playMusic(handle, loop);
}

void AudioManager::pauseMusic() const
{
	if ( !backend ) return;
	backend->pauseMusic();
}

void AudioManager::stopMusic() const
{
	if ( !backend ) return;
	backend->stopMusic();
}


void AudioManager::resumeMusic() const
{
	if ( !backend ) return;
	backend->resumeMusic();
}