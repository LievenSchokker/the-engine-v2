#include "Core/EngineLoops/ClientLoop.h"

#include "Core/EngineLoops/ServerLoop.h"
#include "External/SDLBackendContext.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/TransportGNS.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <ostream>

// TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::move(spel->getSceneManager())),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(spel->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>()))
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

	auto activeScene = sceneManager->getActiveScene();
	if ( activeScene == nullptr )
	{
		// TODO BETERE ERROR
		throw std::runtime_error("No active scene");
	}

	renderer->update(deltaTime, *activeScene);
	RenderQueue renderQueue;
}

void ClientLoop::fixedUpdate(double deltaTime)
{
	client->poll();
	sceneManager->update(deltaTime, gameWorld.get());
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