#pragma once


#include "Core/ApplicationSpecifications.h"
#include "Core/IEngineLoop.h"
#include "External/SdlContext.h"
#include "Rendering/RenderSystem.h"

class Game;
class Client;
class IRenderer;
class InputManager;

#include <functional>
#include <memory>

/**
 * @class ClientLoop
 * @brief Engine loop for interactive clients with rendering and input
 *
 * Handles all player-facing responsibilities: rendering the game state,
 * capturing input, and communicating with the authoritative server. Unlike
 * ServerLoop, this implementation is not authoritative—it predicts game
 * state locally for responsiveness but defers to server corrections.
 *
 *
 * @see ServerLoop, IEngineLoop
 */
class ClientLoop : public IEngineLoop
{
    using ClockFunction = std::function<double()>;

public:
    /**
     * @brief Constructs a client loop with rendering and networking support
     *
     * Defers window creation to start() for two-phase initialization—this
     * allows the engine to configure additional settings between construction
     * and the window becoming visible.
     *
     * @param applicationSpecifications Client configuration (resolution, server address)
     */
	explicit ClientLoop(std::unique_ptr<Game> game);
    ~ClientLoop() override;

    GameWorld* getGameWorld() override;
    SceneManager* getSceneManager() override;
    ClockFunction getClock() override;
    void start() override;
    void update(double deltaTime) override;
    void fixedUpdate(double deltaTime) override;
    void shutdown() override;

private:
    /**
     * @brief Establishes connection to the game server
     *
     * Separated from start() to allow future flexibility in connection
     * timing (e.g., connecting after a menu, reconnecting after disconnect).
     */
    void initializeNetworking();

	std::unique_ptr<Game> game;
    ApplicationSpecifications specifications;
    std::unique_ptr<GameWorld> gameWorld;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<Client> client;
    std::unique_ptr<RenderSystem> renderer;
    std::unique_ptr<SdlContext> sdlContext;
    InputManager* inputManager;
    ClockFunction clockFunction;
};