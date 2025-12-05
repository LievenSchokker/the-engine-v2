#include "Core/EngineLoops/ClientLoop.h"

#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ServerLoop.h"
#include "External/SDLBackendContext.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/Server/Server.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/TransportGNS.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <ostream>

//TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::make_unique<SceneManager>()),
	  game(std::move(spel)),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(game->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>()))
{
	clockFunction = []()
	{
		return 1.0;
	};
	if (specifications.renderBackend == RenderBackend::SDL)
	{
		backendContext = std::make_unique<SDLBackendContext>();
		//TODO SDL Injection layer
		clockFunction = []()
		{
			return SDL_GetTicks() / 1000.0;
		};
		renderer = std::make_unique<SDLRenderer>(*backendContext);

		// Audio
		auto audioBackend = std::make_unique<AudioBackendSDL>();
		audioManager = std::make_unique<AudioManager>();
		audioManager->initialize(std::move(audioBackend));
	}

	inputManager = InputManager::getInstance();
}

ClientLoop::~ClientLoop() = default;

void ClientLoop::start()
{
	renderer->open(specifications.windowOptions);
	initializeNetworking();
}

void ClientLoop::update(double deltaTime)
{
	inputManager->update();
	RenderQueue renderQueue;
	sceneManager->buildRenderQueue(renderQueue);
	renderer->presentFrame();
	client->poll();
}

void ClientLoop::fixedUpdate(double deltaTime)
{
	client->poll();
	sceneManager->update(deltaTime);
	if (inputManager->quitRequested())
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
	renderer->close();
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
	if (sceneManager) return sceneManager.get();
	return nullptr;
}