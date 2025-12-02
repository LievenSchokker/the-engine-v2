#include "Audio/MusicSource.h"
#include "Audio/AudioManager.h"


bool MusicSource::loadMusic(const std::string& path)
{
	if (!audioManager) return false;
	handle = audioManager->loadMusic(path);
	if (handle == -1) return false;
	musicAssetTag = path;
	return true;
}

void MusicSource::play()
{
	if (!audioManager || handle == -1) return;
	audioManager->setMusicSource(this);
	audioManager->setMusicVolume(volume);
	audioManager->playMusic(handle, loop ? -1 : 0);
	playing = true;
}

void MusicSource::stop()
{
	if ( !audioManager ) return;
	audioManager->stopMusic();
	playing = false;
}

void MusicSource::pause()
{
	audioManager->pauseMusic();
	playing = false;
}

void MusicSource::resume()
{
	audioManager->resumeMusic();
	playing = true;
}

void MusicSource::setVolume(float value)
{
	volume = value;
	if ( !audioManager ) return;
	audioManager->setMusicVolume(volume);
}

void MusicSource::setLoop(bool shouldLoop)
{
	loop = shouldLoop;
}
