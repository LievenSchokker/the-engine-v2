#pragma once

<<<<<<< HEAD
class ApplicationClock;
class IPhysicsWorld;
class IRenderer;
class SceneManager;
class Server;
class Client;
class NetworkSpawnManager;
=======

#include "Core/ApplicationSpecifications.h"

class Game;
class IEngineLoop;
class ApplicationClock;

#include <memory>
#include <thread>
>>>>>>> origin/development

#include "Networking/NetworkingIdentityRegistry.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Rendering/RenderQueue.h"

<<<<<<< HEAD
#include <atomic>
#include <memory>
#include <thread>

class SpelMotor
{
public:
    SpelMotor(ApplicationSpecifications applicationSpecifications);
    ~SpelMotor();

    void run();

    SceneManager* getSceneManager();
    GameWorld* getGameWorld() { return &gameWorld; }

private:
    void runClient();
    void runServer();
    void shutdown();

    void initializeNetworking();

    int tickRate;

    const ApplicationSpecifications specifications;

    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<ApplicationClock> timer;
    std::unique_ptr<IPhysicsWorld> physicsWorld;
    std::unique_ptr<SceneManager> sceneManager;

    std::unique_ptr<Server> server;
    std::unique_ptr<Client> client;
    std::unique_ptr<NetworkSpawnManager> spawnManager;
    std::unique_ptr<NetworkIdentityRegistry> identityRegistry;
    GameWorld gameWorld;
=======
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
	* @brief The main game loop that runs until shutdown is requested.
	*
	* Encapsulated as a private method to enforce that the game loop can only be
	* entered through run(), preventing accidental re-entry or misuse.
	*
	*/
	void run();
	/**
	 * @brief Immediately shuts down all engine systems.
	 *
	 * Performs cleanup of all subsystems in the reverse order of their initialization
	 * to prevent dependency issues.
	 *
	 */
	void shutdown() const;

private:
	/** @brief Immutable configuration set at construction.
	 * Const ensures runtime modifications don't destabilize systems.
	 */
	const ApplicationSpecifications specifications;

	/** @brief Tracks whether the game loop is active. */
	bool running;

	std::unique_ptr<IEngineLoop> coreSystemLoop;
	std::unique_ptr<ApplicationClock> coreClock;
>>>>>>> origin/development
};