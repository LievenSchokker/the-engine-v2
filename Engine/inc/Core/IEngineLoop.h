#pragma once

#include "ApplicationClock.h"
#include "GameWorld.h"

#include <functional>

/**
 * @interface IEngineLoop
 * @brief Abstract interface for engine loop implementations
 *
 * This interface exists to support different runtime configurations (client, server,
 * or hybrid) without coupling the core engine to any specific implementation. By
 * programming against this interface, SpelMotor can drive any loop type uniformly,
 * enabling the same timing and update logic to work across all configurations.
 *
 * The split between client and server loops allows each to initialize only the
 * subsystems they need (e.g., servers skip rendering, clients skip authoritative
 * game state) while sharing the same update contract.
 *
 * @see ClientLoop, ServerLoop, SpelMotor
 */
class IEngineLoop
{
	using ClockFunction = std::function<double()>;

public:
	virtual ~IEngineLoop() = default;

	/**
	 * @brief Provides access to the central game state container
	 *
	 * Exposed as a getter rather than passed through update() to allow external
	 * systems (networking, serialization) to access game state outside the
	 * update cycle when needed.
	 */
	virtual GameWorld* getGameWorld() = 0;

	/**
	 * @brief Provides access to the scene management system
	 *
	 * Separated from GameWorld to maintain single responsibility—scenes handle
	 * entity lifecycle and spatial organization, while GameWorld owns the
	 * broader simulation state.
	 */
	virtual SceneManager* getSceneManager() = 0;

	/**
	 * @brief Returns a clock function appropriate for this loop's environment
	 *
	 * Different environments require different clock sources: clients use
	 * SDL_GetTicks for consistency with the rendering subsystem, while servers
	 * use std::steady_clock to avoid SDL dependencies. Returning a function
	 * rather than a value allows ApplicationClock to sample time independently
	 * and remain decoupled from the specific clock implementation.
	 */
	virtual ApplicationClock::ClockFunction getClock() = 0;

	/**
	 * @brief Performs one-time initialization after construction
	 *
	 * Two-phase initialization allows the engine to construct all subsystems
	 * first, then start them in a controlled order. This is necessary because
	 * some systems (like networking) may need other systems to exist before
	 * they can initialize, but shouldn't begin operation until explicitly told.
	 */
	virtual void start() = 0;

	/**
	 * @brief Called every frame for time-sensitive, non-deterministic work
	 *
	 * Intended for rendering, input polling, and other operations that should
	 * run as fast as possible and don't require fixed timing. Receives delta
	 * time for interpolation purposes (e.g., smoothing between physics states).
	 *
	 * @param fixedDeltaTime Time step for interpolation calculations
	 */
	virtual void update(double fixedDeltaTime) = 0;

	/**
	 * @brief Called at a fixed rate for deterministic simulation
	 *
	 * Physics, game logic, and networking updates belong here to ensure
	 * consistent, reproducible behavior regardless of frame rate. The fixed
	 * timestep guarantees that the same inputs produce the same outputs,
	 * which is essential for networked games and replay systems.
	 *
	 * @param fixedDeltaTime Guaranteed constant time step between calls
	 */
	virtual void fixedUpdate(double fixedDeltaTime) = 0;

	/**
	 * @brief Performs explicit cleanup before destruction
	 *
	 * Separated from the destructor to allow controlled shutdown ordering
	 * and to handle cleanup that may fail or require logging. Destructors
	 * should not throw, but shutdown operations (like disconnecting from
	 * a server) may need error handling.
	 */
	virtual void shutdown() = 0;

	/**
	 * @brief Checks if shutdown has been requested
	 *
	 * Allows SpelMotor to detect when the loop has initiated shutdown
	 * and stop the main loop gracefully. Default implementation returns
	 * false for loops that don't support shutdown detection.
	 *
	 * @return true if shutdown has been requested, false otherwise
	 */
	virtual bool isShutdownRequested() const
	{
		return false;
	}
};