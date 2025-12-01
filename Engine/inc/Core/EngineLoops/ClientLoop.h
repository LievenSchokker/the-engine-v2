#pragma once


#include "Core/IEngineLoop.h"
#include "External/SdlContext.h"

#include <functional>
#include <memory>

class ClientLoop: public IEngineLoop
{
	using ClockFunction = std::function<double()>;

public:
	explicit ClientLoop(
		const ApplicationSpecifications& applicationSpecifications);
	~ClientLoop() override = default;

	GameWorld* getGameWorld() override;
	SceneManager* getSceneManager() override;
	ClockFunction getClock() override;
	void start() override;
	void initializeNetworking();
	void update(double deltaTime) override;
	void fixedUpdate(double deltaTime) override;
	void shutdown() override;

private:
	ApplicationSpecifications applicationSpecifications;
	std::unique_ptr<GameWorld> gameWorld;
	std::unique_ptr<SceneManager> sceneManager;
	std::unique_ptr<Client> client;
	std::unique_ptr<IRenderer> renderer;
	std::unique_ptr<SdlContext> sdlContext;
	std::unique_ptr<ClockFunction> clock;
	InputManager* inputManager;
	ClockFunction clockFunction;
};