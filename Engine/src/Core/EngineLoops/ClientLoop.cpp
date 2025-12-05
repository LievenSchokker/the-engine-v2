#include "Core/EngineLoops/ClientLoop.h"

#include "Game.h"
#include "Core/ApplicationClock.h"
#include "Core/EngineLoops/ServerLoop.h"
#include "Events/ApplicationEvents.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <ostream>
#include "Networking/Server/Server.h"

//TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(std::unique_ptr<Game> spel)
	: sceneManager(std::make_unique<SceneManager>()),
	  game(std::move(spel)),
	  gameWorld(std::make_unique<GameWorld>()),
	  specifications(game->getApplicationSpecifications()),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>())),
	  inputManager(nullptr)
{
	clockFunction = []()
	{
		return 1.0;
	};
	if (specifications.renderBackend == RenderBackend::SDL)
	{
		sdlContext = std::make_unique<SdlContext>();
		//TODO SDL Injection layer
		clockFunction = []()
		{
			return SDL_GetTicks() / 1000.0;
		};
		std::unique_ptr<IRenderer> sdlRenderer = std::make_unique<SDLRenderer>(
			*sdlContext);
		sdlRenderer->open(specifications.windowOptions);
		renderer = std::make_unique<RenderSystem>(std::move(sdlRenderer));
	}
	std::unique_ptr<Scene> scenePtr = game->getFirstScene();
	std::string scene = scenePtr->getName();

	inputManager = InputManager::getInstance();
	inputManager->initialize(eventDispatcher);

	sceneManager->addScene(std::move(scenePtr));
	sceneManager->setActiveScene(scene);
}

ClientLoop::~ClientLoop() = default;

void ClientLoop::start()
{
	initializeNetworking();
	initializeEvents();
}


void ClientLoop::initializeEvents()
{
	eventDispatcher.subscribe<KeyPressedEvent>(
		[this](const KeyPressedEvent& e) {
			if (e.keyCode == SDLK_ESCAPE && !e.isRepeat)
			{
				shutdown();
			}
		});
}

void ClientLoop::update(double deltaTime)
{
	inputManager->beginFrame();

	sdlEventProcessor.pollEvents(eventQueue);
	eventQueue.processAll(eventDispatcher);

	inputManager->endFrame();

	renderer->update(deltaTime, *sceneManager->getActiveScene());
	RenderQueue renderQueue;
}

void ClientLoop::fixedUpdate(double deltaTime)
{
	client->poll();
	sceneManager->update(deltaTime, gameWorld.get());
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
	eventDispatcher.unsubscribe(windowCloseHandle);
	eventDispatcher.unsubscribe(windowResizeHandle);

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
	if (sceneManager) return sceneManager.get();
	return nullptr;
}

EventDispatcher& ClientLoop::getEventDispatcher()
{
	return eventDispatcher;
}

EventQueue& ClientLoop::getEventQueue()
{
	return eventQueue;
}
