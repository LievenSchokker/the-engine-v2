#include "Core/EngineLoops/ClientLoop.h"

#include "Audio/Components/MusicSource.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/EngineLoops/ServerLoop.h"
#include "External/SDLBackendContext.h"
#include "Game.h"
#include "Game.h"
#include "Core/ApplicationClock.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Networking/Client.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/TransportGNS.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/SceneManager.h"

ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::move(spel->getSceneManager())),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(spel->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>())),
	  isShutdown(false)
{
	gameWorld->sceneManager = sceneManager.get();

	clockFunction = []() { return 1.0; };
	if ( specifications.renderBackend == RenderBackend::SDL )
	{
		backendContext = std::make_unique<SDLBackendContext>();
		clockFunction = []() { return SDL_GetTicks() / 1000.0; };
		std::unique_ptr<IRenderer> sdlRenderer =
			std::make_unique<SDLRenderer>(*backendContext);

		sdlRenderer->open(specifications.windowOptions);
		renderer = std::make_unique<RenderSystem>(std::move(sdlRenderer));
		gameWorld->render = renderer.get();
	}

	gameWorld->sceneManager = sceneManager.get();
	gameWorld->input = InputManager::getInstance();
	inputManager = InputManager::getInstance();

	auto backend = std::make_unique<AudioBackendSDL>();
	audioManager = std::make_unique<AudioManager>();


	gameWorld->input = InputManager::getInstance();
	gameWorld->client = client.get();
	gameWorld->sceneManager = sceneManager.get();

	spawnManager = std::make_unique<NetworkSpawnManager>(gameWorld.get());
	gameWorld->spawnManager = spawnManager.get();

	sceneManager->configureNetworking(ConnectionMode::Client, spawnManager.get());

    if (sceneManager->getActiveScene() == nullptr)
    {
        std::string sceneName = sceneManager->getFirstSceneName();
        if (!sceneName.empty())
        {
            sceneManager->setActiveScene(sceneName);
        }
    }
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

	if (!client->connectToServer(serverInfo))
	{
		std::cerr << "Failed to connect to server!" << std::endl;
		return;
	}

	auto dispatcher =
		spelmotorNetworking::MessageDispatcherFactory::createClientDispatcher(
			*gameWorld,
			*spawnManager,
			spawnManager->getNetworkIdentityRegistry());
	client->injectMessageDispatcher(std::move(dispatcher));
}

void ClientLoop::shutdown()
{
	isShutdown = true;
	InputManager::shutdown();
	renderer.release();
	client->disconnect();
	InputManager::shutdown();
	renderer.reset();
}

bool ClientLoop::isShutdownRequested() const
{
	return isShutdown;
}


GameWorld* ClientLoop::getGameWorld()
{
	return gameWorld.get();
}

SceneManager* ClientLoop::getSceneManager()
{
    if ( sceneManager ) return sceneManager.get();
    return nullptr;
}

ClientLoop::ClockFunction ClientLoop::getClock()
{
	return clockFunction;
}

void ClientLoop::setApplicationClock(ApplicationClock* clock)
{
	if ( gameWorld )
	{
		gameWorld->clock = clock;
	}
}