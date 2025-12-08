#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Core/IEngineLoop.h"

class Game;
class SceneManager;
class Server;

#include <functional>
#include <memory>

/**
 * @class ServerLoop
 * @brief Headless engine loop for authoritative game servers
 *
 * This implementation strips away all client-specific subsystems (rendering,
 * audio, local input) to run a dedicated server with minimal resource overhead.
 * Servers are authoritative—they own the true game state and validate all
 * client actions, making them the source of truth in networked games.
 *
 * Runs headless to allow deployment on machines without graphics capabilities
 * and to maximize tick rate by avoiding rendering overhead. The same fixed
 * timestep logic ensures server and client simulations stay synchronized.
 *
 * @see ClientLoop, IEngineLoop
 */
class ServerLoop: public IEngineLoop
{
	using ClockFunction = std::function<double()>;

public:
	/**
	 * @brief Constructs a server loop with the provided configuration
	 *
	 * Uses std::steady_clock rather than SDL timing to avoid pulling in SDL
	 * as a dependency for headless servers. steady_clock is guaranteed
	 * monotonic, preventing issues if system time is adjusted during runtime.
	 *
	 * @param applicationSpecifications Server configuration (tick rate, network settings)
	 */
	explicit ServerLoop(std::unique_ptr<Game> game);
	~ServerLoop() override;

	GameWorld* getGameWorld() override;
	SceneManager* getSceneManager() override;
	ClockFunction getClock() override;
	void start() override;
	void update(double deltaTime) override;
	void fixedUpdate(double deltaTime) override;
	void shutdown() override;

private:
	/// Stored to allow runtime access to configuration (e.g., for network settings)
	ApplicationSpecifications specifications;
	std::unique_ptr<SceneManager> sceneManager;
	std::unique_ptr<Server> server;
	std::unique_ptr<GameWorld> gameWorld;
	std::unique_ptr<Client> game;
	ClockFunction clockFunction;
	uint32_t currentTick = 0;
};