#include "Core/EngineLoops/ClientLoop.h"

#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ServerLoop.h"
#include "External/SDLBackendContext.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/TransportGNS.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

// TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::make_unique<SceneManager>()),
	  game(std::move(spel)),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(game->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>()))
{
	clockFunction = []() { return 1.0; };
	if ( specifications.renderBackend == RenderBackend::SDL )
	{
		backendContext = std::make_unique<SDLBackendContext>();
		// TODO SDL Injection layer
		clockFunction = []() { return SDL_GetTicks() / 1000.0; };
		std::unique_ptr<IRenderer> sdlRenderer =
			std::make_unique<SDLRenderer>(*backendContext);
		sdlRenderer->open(specifications.windowOptions);
		renderer = std::make_unique<RenderSystem>(std::move(sdlRenderer));

		// Audio
		auto audioBackend = std::make_unique<AudioBackendSDL>();
		audioManager = std::make_unique<AudioManager>();
		audioManager->initialize(std::move(audioBackend));
	}
	std::unique_ptr<Scene> scenePtr = game->getFirstScene();
	std::string scene = scenePtr->getName();

	sceneManager->addScene(std::move(scenePtr));
	sceneManager->setActiveScene(scene);

	inputManager = InputManager::getInstance();
}

ClientLoop::~ClientLoop() = default;

void ClientLoop::start()
{
	initializeNetworking();
}

void ClientLoop::update(double deltaTime)
{
	inputManager->update();

	// Update behaviors unconditionally (even when paused) so debug controls
	// work This allows behaviors to handle input that needs to work when paused
	sceneManager->updateAlways(deltaTime, gameWorld.get());

	// Handle ESC key and window close - check in update() so it works when
	// paused
	if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) ||
		 inputManager->quitRequested() )
	{
		shutdown();
		return;
	}

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
	InputManager::shutdown();
	renderer.release();
	client->disconnect();
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