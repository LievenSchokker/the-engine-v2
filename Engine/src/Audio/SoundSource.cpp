#include "Audio/SoundSource.h"
#include "Audio/AudioManager.h"

bool SoundSource::loadSound(const std::string& path)
{
	if (!audioManager) return false;

	handle = audioManager->loadSound(path);
	if (handle < 0) return false;

	soundAssetTag = path;
	return true;
}

void SoundSource::play(int loops)
{
	if (!audioManager || handle < 0) return;

	// Ask AudioManager to play sound
	channel = audioManager->playSound(handle, loops);
}

void SoundSource::stop()
{
	if (!audioManager || channel < 0) return;

	audioManager->stopChannel(channel);
}

void SoundSource::setPanning(float left, float right)
{
	if (!audioManager || channel < 0) return;

	audioManager->setChannelPanning(channel, left, right);
}
