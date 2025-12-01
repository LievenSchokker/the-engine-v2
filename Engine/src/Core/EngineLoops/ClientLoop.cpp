#include "Core/EngineLoops/ClientLoop.h"
#include "Core/ApplicationClock.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <ostream>


//TODO Create proper factory for each system that needs to be created
ClientLoop::ClientLoop(
	const ApplicationSpecifications& applicationSpecifications)
	: sceneManager(std::make_unique<SceneManager>()),
	  gameWorld(std::make_unique<GameWorld>()),
	  applicationSpecifications(applicationSpecifications),
	  client(std::make_unique<Client>(std::make_unique<TransportGNS>()))
{
	clockFunction = []()
	{
		return 1.0;
	};
	if (applicationSpecifications.renderBackend == RenderBackend::SDL)
	{
		sdlContext = std::make_unique<SdlContext>();
		//TODO SDL Injection layer
		clockFunction = []()
		{
			return SDL_GetTicks() / 1000.0;
		};
		renderer = std::make_unique<SDLRenderer>(*sdlContext);
	}

	inputManager = InputManager::getInstance();
}

void ClientLoop::start()
{
	renderer->open(applicationSpecifications.windowOptions);
	initializeNetworking();
}

void ClientLoop::update()
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
	serverInfo.ip = applicationSpecifications.networkingOptions.serverIP;
	serverInfo.port = applicationSpecifications.networkingOptions.port;
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