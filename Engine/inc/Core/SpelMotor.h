#pragma once
#include "Options/ApplicationSpecifications.h"

class Game;
class IEngineLoop;
class ApplicationClock;

#include <memory>

/**
 * @class SpelMotor
 * @brief Core engine class that manages the game loop and system lifecycle.
 *
 * SpelMotor acts as the orchestrator for the entire engine managing
 * the lifetime cycle of all engine systems.
 *
 */

class SpelMotor
{
   public:
	explicit SpelMotor(std::unique_ptr<Game> game);
    SpelMotor(std::unique_ptr<Game> game, std::unique_ptr<IEngineLoop> engineLoop);
	~SpelMotor();
	/**
	 * @brief Starts the engine and enters the main game loop.
	 *
	 * This method performs all system initialization (rendering, input, etc.)
	 * and then enters the BLOCKING update loop. It only returns when the engine
	 * has been shut down.
	 *
	 */
	void start();
	/**
	 * @brief The main game loop that runs until shutdown is requested.
	 *
	 * Encapsulated as a private method to enforce that the game loop can only
	 * be entered through run(), preventing accidental re-entry or misuse.
	 *
	 */
	void run();
	/**
	 * @brief Immediately shuts down all engine systems.
	 *
	 * Performs cleanup of all subsystems in the reverse order of their
	 * initialization to prevent dependency issues.
	 *
	 */
	void shutdown();

	/**
	 * @brief Gets the application clock for debug time controls.
	 *
	 * Allows access to time scale and pause functionality for debugging.
	 * Use with caution in production code.
	 *
	 * @return Pointer to the application clock
	 */
	ApplicationClock* getClock();

   private:
	/** @brief Immutable configuration set at construction.
	 * Const ensures runtime modifications don't destabilize systems.
	 */
	const ApplicationSpecifications specifications;

	/** @brief Tracks whether the game loop is active. */
	mutable bool running;

    std::unique_ptr<IEngineLoop> coreSystemLoop;
    std::unique_ptr<ApplicationClock> coreClock;
};