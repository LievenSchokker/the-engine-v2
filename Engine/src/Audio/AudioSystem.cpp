
#include "Audio/AudioSystem.h"

#include "Audio/AudioManager.h"
#include "Audio/IAudioBackend.h"
#include "Core/GameWorld.h"

AudioSystem::AudioSystem(std::unique_ptr<IAudioBackend> backend)
	: audioManager(std::make_unique<AudioManager>())
	, backend(std::move(backend))
{
}

AudioSystem::~AudioSystem() = default;

SystemStatus AudioSystem::start(GameWorld& gameWorld)
{
	audioManager->initialize(std::move(backend));
	return SystemStatus::Running;
}

void AudioSystem::update(double deltaTime, const GameWorld& gameWorld)
{
	// Future: update audio listeners, 3D positioning, etc.
}

void AudioSystem::shutdown(GameWorld& gameWorld)
{
	audioManager->shutdown();
}