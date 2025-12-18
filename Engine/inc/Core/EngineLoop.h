#pragma once

#include "Core/IEngineLoop.h"
#include "External/IBackendContext.h"

#include <functional>
#include <memory>
#include <vector>

class IEngineSystems;
class Game;
class GameWorld;
class SceneManager;
class ApplicationClock;
struct ApplicationSpecifications;

/**
 * @class EngineLoop
 * @brief Core engine loop implementation that manages game systems and the main update cycle.
 *
 * The EngineLoop is responsible for orchestrating all engine subsystems, managing their
 * lifecycle (initialization, update, shutdown), and providing access to core engine
 * components like the GameWorld, SceneManager, and backend context.
 *
 * @note Systems are updated in the order they are added and shutdown in reverse order.
 *
 * @see IEngineLoop
 * @see IEngineSystem
 */
class EngineLoop: public IEngineLoop
{
public:
	/**
	 * @brief Function type for retrieving the current time.
	 */
	using ClockFunction = std::function<double()>;

	/**
	 * @brief Constructs an EngineLoop with the specified game instance.\
	 * This should only be used by the factory.Trying to create your own EngineLoop might
	 * break the engine.
	 *
	 * @param game Unique pointer to the Game instance. Ownership is transferred to EngineLoop.
	 */
	explicit EngineLoop(std::unique_ptr<Game> game);

	/**
	 * @brief Destructor. Cleans up all registered systems and resources.
	 */
	~EngineLoop() override;

	/**
	 * @brief Registers an engine system with the loop.
	 *
	 * Systems are updated in the order they are added. Ownership is transferred
	 * to the EngineLoop.
	 *
	 * @param system Unique pointer to the system to add.
	 */
	void addSystem(std::unique_ptr<IEngineSystems> system);

	/**
	 * @brief Initializes all registered systems.
	 *
	 * Called once before the main loop begins. Systems are started in the
	 * order they were added.
	 */
	void start() override;

	/**
	 * @brief Performs a variable timestep update on all systems.
	 *
	 * Called every frame with the time elapsed since the last frame.
	 *
	 * @param deltaTime Time in seconds since the last update.
	 */
	void update(double deltaTime) override;

	/**
	 * @brief Performs a fixed timestep update on all systems.
	 *
	 * Called at a fixed interval for deterministic physics and gameplay logic.
	 *
	 * @param deltaTime Fixed time step in seconds.
	 */
	void fixedUpdate(double deltaTime) override;

	/**
	 * @brief Shuts down all registered systems.
	 *
	 * Systems are shut down in reverse order of registration to ensure
	 * proper dependency cleanup.
	 */
	void shutdown() override;

	/**
	 * @brief Checks if a shutdown has been requested.
	 *
	 * @return True if shutdown has been requested, false otherwise.
	 */
	[[nodiscard]] bool isShutdownRequested() const override;

	/**
	 * @brief Retrieves the clock function used for timing.
	 *
	 * @return The current clock function.
	 */
	ClockFunction getClock() override;

	/**
	 * @brief Retrieves the game world instance.
	 *
	 * @return Pointer to the GameWorld, or nullptr if not initialized.
	 */
	GameWorld* getGameWorld() override;

	void setGameWorld(std::unique_ptr<GameWorld> gameWorld) override;
	/**
	 * @brief Retrieves the scene manager.
	 *
	 * @return Pointer to the SceneManager, or nullptr if not initialized.
	 */
	SceneManager* getSceneManager() override;

	/**
	 * @brief Retrieves the game instance.
	 *
	 * @return Pointer to the Game instance owned by this loop.
	 */
	[[nodiscard]] Game* getGame() const;

	/**
	 * @brief Retrieves a copy of the application specifications.
	 *
	 * @return Copy of the ApplicationSpecifications from the Game instance.
	 */
	[[nodiscard]] ApplicationSpecifications getSpecifications() const;

	/**
	 * @brief Retrieves the backend context.
	 *
	 * @return Pointer to the backend context, or nullptr if not set.
	 */
	[[nodiscard]] IBackendContext* getBackendContext() const;

	/**
	 * @brief Retrieves all registered systems.
	 *
	 * @return Const reference to the vector of registered systems.
	 */
	[[nodiscard]] const std::vector<std::unique_ptr<IEngineSystems>>&
	getSystems() const;

	/**
	 * @brief Sets the application clock for the game world.
	 *
	 * @param clock Pointer to the ApplicationClock instance.
	 */
	void setApplicationClock(ApplicationClock* clock) override;

	/**
	 * @brief Sets the clock function used for timing.
	 *
	 * @param func Function that returns the current time in seconds.
	 *
	 * @code
	 * loop->setClockFunction([]() { return SDL_GetTicks() / 1000.0; });
	 * @endcode
	 */
	void setClockFunction(ClockFunction func);

	/**
	 * @brief Sets the backend context for platform-specific functionality.
	 *
	 * @param context Unique pointer to the backend context. Ownership is transferred.
	 */
	void setBackendContext(std::unique_ptr<IBackendContext> context);

	/**
	 * @brief Requests the engine loop to shut down.
	 *
	 * The shutdown will occur at the end of the current frame.
	 */
	void requestShutdown();

private:
	std::unique_ptr<IBackendContext> backendContext;
	std::unique_ptr<Game> game;
	std::unique_ptr<GameWorld> gameWorld;
	std::vector<std::unique_ptr<IEngineSystems>> systems;
	SceneManager* sceneManagerPtr;
	ClockFunction clockFunction;
	bool shutdownRequested = false;
};