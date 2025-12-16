#pragma once

#include "Core/IEngineLoop.h"
#include "External/IBackendContext.h"

#include <functional>
#include <memory>
#include <vector>

class IEngineSystem;
class Game;
class GameWorld;
class SceneManager;
class ApplicationClock;
struct ApplicationSpecifications;

class EngineLoop : public IEngineLoop
{
public:
	using ClockFunction = std::function<double()>;

	explicit EngineLoop(std::unique_ptr<Game> game);
	~EngineLoop() override;

	void addSystem(std::unique_ptr<IEngineSystem> system);

	template<typename T>
	T* getSystem()
	{
		for (auto& sys : systems)
		{
			if (auto* casted = dynamic_cast<T*>(sys.get()))
			{
				return casted;
			}
		}
		return nullptr;
	}

	void start() override;
	void update(double deltaTime) override;
	void fixedUpdate(double deltaTime) override;
	void shutdown() override;
	bool isShutdownRequested() const override;

	ClockFunction getClock() override;
	GameWorld* getGameWorld() override;
	SceneManager* getSceneManager() override;
	void setApplicationClock(ApplicationClock* clock) override;


	Game* getGame() { return game.get(); }
	const ApplicationSpecifications getSpecifications() const;
	void setClockFunction(ClockFunction func) { clockFunction = std::move(func); }
	void requestShutdown() { shutdownRequested = true; }

	void setBackendContext(std::unique_ptr<IBackendContext> context)
	{
		backendContext = std::move(context);
	}
private:
	std::unique_ptr<IBackendContext> backendContext;
	std::unique_ptr<Game> game;
	std::unique_ptr<GameWorld> gameWorld;
	std::vector<std::unique_ptr<IEngineSystem>> systems;

	ClockFunction clockFunction;
	bool shutdownRequested = false;

	// Cached pointers for quick access
	SceneManager* sceneManagerPtr = nullptr;
};