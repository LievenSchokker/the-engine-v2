#include "Audio/Components/SoundSource.h"

#include "Audio/AudioManager.h"

SoundSource::SoundSource(AudioManager* audioManagerPtr)
	: audioManager(audioManagerPtr)
{
}

bool SoundSource::loadSound(const std::string& newpath)
{
	if ( !audioManager ) return false;

	path = newpath;
	return audioManager->loadSound(path);
}

void SoundSource::play(int loops)
{
	if ( !audioManager ) return;

	channel = audioManager->playSound(path, loops, lastLeft, lastRight);
}

void SoundSource::stop() const
{
	if ( !audioManager || channel < 0 ) return;

	audioManager->stopChannel(channel);
}

void SoundSource::setPanning(float left, float right)
{
	lastLeft = left;
	lastRight = right;
}

void SoundSource::setCurrentPanning(float left, float right) const
{
	audioManager->setChannelPanning(channel, left, right);
}