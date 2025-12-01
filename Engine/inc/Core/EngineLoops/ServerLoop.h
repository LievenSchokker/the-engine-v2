#pragma once

#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Core/IEngineLoop.h"
#include <functional>
#include <memory>

class SceneManager;

class ServerLoop: public IEngineLoop
{
	using ClockFunction = std::function<double()>;

public:
	explicit ServerLoop(
		const ApplicationSpecifications& applicationSpecifications);
	~ServerLoop() override = default;

	GameWorld* getGameWorld() override;
	SceneManager* getSceneManager() override;
	ClockFunction getClock() override;
	void start() override;
	void update(double deltaTime) override;
	void fixedUpdate(double deltaTime) override;
	void shutdown() override;

private:
	ApplicationSpecifications applicationSpecifications;
	std::unique_ptr<SceneManager> sceneManager;
	std::unique_ptr<Server> server;
	std::unique_ptr<GameWorld> gameWorld;
	ClockFunction clockFunction;
	uint32_t currentTick = 0;
};