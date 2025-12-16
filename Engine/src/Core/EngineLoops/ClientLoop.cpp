#include "Core/EngineLoops/ClientLoop.h"

#include "Core/EngineLoops/ServerLoop.h"
#include "External/SDLBackendContext.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Networking/Client.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/TransportGNS.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/SceneManager.h"

// TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::move(spel->getSceneManager())),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(spel->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>())),
	  isShutdown(false)
{
	// Transfer ownership of the scene manager and give gameWorld a non owning
	// pointer
	gameWorld->sceneManager = sceneManager.get();

	clockFunction = []() { return 1.0; };
	if ( specifications.renderBackend == RenderBackend::SDL )
	{
		// TODO SDL Injection layer
		backendContext = std::make_unique<SDLBackendContext>();
		clockFunction = []() { return SDL_GetTicks() / 1000.0; };
		std::unique_ptr<IRenderer> sdlRenderer =
			std::make_unique<SDLRenderer>(*backendContext);

		sdlRenderer->open(specifications.windowOptions);
		renderer = std::make_unique<RenderSystem>(std::move(sdlRenderer));
		gameWorld->render = renderer.get();
	}

	// Set GameWorld references for behaviors to access
	gameWorld->sceneManager = sceneManager.get();
	gameWorld->input = InputManager::getInstance();
	inputManager = InputManager::getInstance();
}

ClientLoop::~ClientLoop() = default;

void ClientLoop::start()
{
	initializeNetworking();
}

void ClientLoop::update(double deltaTime)
{
	if ( !renderer )
	{
		return;
	}
	inputManager->update();

	auto activeScene = sceneManager->getActiveScene();
	if ( activeScene == nullptr )
	{
		throw std::runtime_error(
			"ClientLoop::update(): No active scene available. Ensure at least "
			"one scene is registered and active.");
	}

	renderer->update(deltaTime, *activeScene);
	// Update behaviors unconditionally (even when paused) so debug controls
	// work This allows behaviors to handle input that needs to work when paused
	sceneManager->updateAlways(deltaTime, gameWorld.get());

	renderer->update(deltaTime, *sceneManager->getActiveScene());
	RenderQueue renderQueue;
}

void ClientLoop::fixedUpdate(double deltaTime)
{
	client->poll();
	// Note: SceneManager::update() removed - behaviors now run from
	// updateAlways() in update() to ensure they run every frame (even when
	// paused) for input handling sceneManager->update(deltaTime,
	// gameWorld.get());
	if ( inputManager->quitRequested() )
	{
		shutdown();
	}
}

void ClientLoop::initializeNetworking()
{
	ServerConnectionInformation serverInfo;
	serverInfo.ip = specifications.networkingOptions.serverIP;
	serverInfo.port = specifications.networkingOptions.port;
	client->connectToServer(serverInfo);
}

void ClientLoop::shutdown()
{
	isShutdown = true;
	InputManager::shutdown();
	renderer.release();
	client->disconnect();
}

bool ClientLoop::isShutdownRequested() const
{
	return isShutdown;
}

GameWorld* ClientLoop::getGameWorld()
{
	return gameWorld.get();
}

ClientLoop::ClockFunction ClientLoop::getClock()
{
	return clockFunction;
}

SceneManager* ClientLoop::getSceneManager()
{
	if ( sceneManager ) return sceneManager.get();
	return nullptr;
}

void ClientLoop::setApplicationClock(ApplicationClock* clock)
{
	if ( gameWorld )
	{
		gameWorld->clock = clock;
	}
}