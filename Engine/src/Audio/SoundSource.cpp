#include "Audio/SoundSource.h"

#include "Audio/AudioManager.h"

SoundSource::SoundSource(AudioManager* audioManagerPtr)
	: audioManager(audioManagerPtr)
{
}

bool SoundSource::loadSound(const std::string& path)
{
	if ( !audioManager ) return false;

	handle = audioManager->loadSound(path);
	if ( handle < 0 ) return false;

	soundAssetTag = path;
	return true;
}

void SoundSource::play(int loops)
{
	if ( !audioManager || handle < 0 ) return;

	// Ask AudioManager to play sound
	channel = audioManager->playSound(handle, loops);
	audioManager->setChannelPanning(channel, lastLeft, lastRight);
}

void SoundSource::stop()
{
	if ( !audioManager || channel < 0 ) return;

	audioManager->stopChannel(channel);
}

void SoundSource::setPanning(float left, float right)
{
	lastLeft = left;
	lastRight = right;

	if ( !audioManager || channel < 0 ) return;

	audioManager->setChannelPanning(channel, left, right);
}