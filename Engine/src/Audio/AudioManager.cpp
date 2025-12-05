#include "Audio/AudioManager.h"

#include "Audio//Components/MusicSource.h"
#include "Audio/IAudioBackend.h"

bool AudioManager::initialize(std::unique_ptr<IAudioBackend> backendPtr)
{
	backend = std::move(backendPtr);
	if ( !backend->initialize() ) return false;
	return true;
}

void AudioManager::shutdown()
{
	audioSources.clear();
	musicSource = nullptr;
	listener = nullptr;
	backend = nullptr;
}


bool AudioManager::loadMusic(const std::string& path) const
{
	return backend->loadMusic(path);
	;
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

void AudioManager::setSoundVolume(float volume)
{
	soundVolume = volume;
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

SoundHandle AudioManager::loadSound(const std::string& path) const
{
	if ( !backend ) return -1;
	return backend->loadSound(path);
}

int AudioManager::playSound(SoundHandle handle, int loops, float left,
							float right) const
{
	if ( !backend ) return -1;

	// Ask backend to choose a free channel
	int channel = backend->reserveFreeChannel();
	if ( channel < 0 ) return -1;

	backend->setChannelPanning(channel, left, right);
	backend->playSound(handle, channel, loops);
	return channel;
}

void AudioManager::stopChannel(int channel) const
{
	if ( !backend ) return;
	backend->stopChannel(channel);
}

void AudioManager::setChannelPanning(int channel, float left, float right) const
{
	if ( !backend ) return;
	backend->setChannelPanning(channel, left, right);
}