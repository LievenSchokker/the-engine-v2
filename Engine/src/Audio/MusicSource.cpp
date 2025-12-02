#include "Audio/MusicSource.h"

#include "Audio/AudioAssetManager.h"
#include "Audio/AudioManager.h"
#include "Audio/IAudioBackend.h"
#include "SDL_mixer.h"

bool MusicSource::loadMusic(const std::string& path)
{
	if ( !audioAssetManager ) return false;

	// Load via the asset manager
	handle = audioAssetManager->loadMusic(path);

	if ( handle == -1 ) return false;

	musicAssetTag = path;
	return true;
}

void MusicSource::play()
{
	if ( !audioManager ) return;

	if ( handle == -1 ) return;

	audioManager->setMusicSource(this);
	audioManager->playMusic(handle, loop ? -1 : 0);
	audioManager->setMusicVolume(volume);
}

void MusicSource::stop() const
{
	if ( !audioManager ) return;
	audioManager->stopMusic();
}

void MusicSource::pause()
{
	// #TODO AudioManager::pauseMusic()
	Mix_PauseMusic();
}

void MusicSource::resume()
{
	// #TODO AudioManager::resumeMusic()
	Mix_ResumeMusic();
}

void MusicSource::setVolume(float value)
{
	volume = value;
	if ( !audioManager ) return;
	audioManager->setMusicVolume(volume);
}
