#include "Audio/AudioSystem.h"

#include "Audio/AudioManager.h"
#include "Core/GameWorld.h"

AudioSystem::AudioSystem(std::unique_ptr<IAudioBackend> backend)
	: audioManager_(std::make_unique<AudioManager>())
	, backend_(std::move(backend))
{
}

AudioSystem::~AudioSystem() = default;

void AudioSystem::start(GameWorld& gameWorld)
{
	audioManager_->initialize(std::move(backend_));
	gameWorld.audio = audioManager_.get();
}

void AudioSystem::update(float deltaTime, const GameWorld& gameWorld)
{
	// Future: update audio listeners, 3D positioning, etc.
}

void AudioSystem::shutdown(GameWorld& gameWorld)
{
	gameWorld.audio = nullptr;
	audioManager_->shutdown();
}