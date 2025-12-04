#pragma once


#include "ApplicationSpecifications.h"
#include "Physics/IPhysicsWorld.h"
#include "Scene/SceneManager.h"

#include <functional>
#include <memory>

class Game;
class ApplicationClock;
class IRenderer;

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

	~SpelMotor();

	/**
	 * @brief Starts the engine and enters the main game loop.
	 *
	 * This method performs all system initialization (rendering, input, etc.) and then
	 * enters the BLOCKING update loop.
	 * It only returns when the engine has been shut down.
	 *
	 */
	void start();

	/**
	 * @brief Immediately shuts down all engine systems.
	 *
	 * Performs cleanup of all subsystems in the reverse order of their initialization
	 * to prevent dependency issues.
	 *
	 */
	void shutdown();

private:
	void initFirstGameScene() const;

	std::unique_ptr<Game> game;

	/** @brief Immutable configuration set at construction.
	 * Const ensures runtime modifications don't destabilize systems. */
	const ApplicationSpecifications specifications;
	/**
	 * @brief The main game loop that runs until shutdown is requested.
	 *
	 * Encapsulated as a private method to enforce that the game loop can only be
	 * entered through run(), preventing accidental re-entry or misuse.
	 *
	 */
	void run();

	int tickRate;
	/** @brief Tracks whether the game loop is active. */
	bool running;

	/** @brief renderer handle. */
	std::unique_ptr<IRenderer> renderer;

	/** @brief timeStep calculation for engine */
	std::unique_ptr<ApplicationClock> timer;

	/** @brief A functions that retusn, the time the applicationhas beenrunning in second */
	std::function<double()> clockFunction;

	/** @brief the physics world where physics are simulated */
	std::unique_ptr<IPhysicsWorld> physicsWorld;

	std::unique_ptr<SceneManager> sceneManager;
};