#pragma once


#include "Core/IEngineSystems.h"

#include <memory>

class AudioManager;
class IAudioBackend;

class AudioSystem : public IEngineSystem
{
public:
	explicit AudioSystem(std::unique_ptr<IAudioBackend> backend);
	~AudioSystem() override;

	SystemStatus start(GameWorld& gameWorld) override;
	void update(double deltaTime, const GameWorld& gameWorld) override;
	void shutdown(GameWorld& gameWorld) override;

	const std::string getName() const override { return "AudioSystem"; }

	AudioManager* getAudioManager() { return audioManager.get(); }

private:
	std::unique_ptr<AudioManager> audioManager;
	std::unique_ptr<IAudioBackend> backend;
};