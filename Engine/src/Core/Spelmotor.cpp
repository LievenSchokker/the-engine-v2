#include "Core/SpelMotor.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Networking/Server/Server.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

#include "Scene/SceneManager.h"

SpelMotor::SpelMotor(ApplicationSpecifications applicationSpecifications)
	: specifications(applicationSpecifications)
	  , sceneManager(std::make_unique<SceneManager>())
{
	const auto& netOpts = specifications.networkingOptions;

	if (netOpts.mode == EngineMode::CLIENT)
	{
		if (specifications.renderBackend == RenderBackend::SDL)
		{
			SdlContext context = SdlContext();
			renderer = std::make_unique<SDLRenderer>(context);
		}

		client = std::make_unique<Client>(std::make_unique<TransportGNS>());
		gameWorld.renderer = renderer.get();
		gameWorld.client = client.get();
	}
	else
	{
		ServerConnectionInformation serverInfo;
		serverInfo.port = netOpts.port;
		server = std::make_unique<Server>(serverInfo,
		                                  std::make_unique<TransportGNS>());

		gameWorld.server = server.get();
	}

	gameWorld.sceneManager = sceneManager.get();
	gameWorld.physics = physicsWorld.get();
	gameWorld.input = InputManager::getInstance();

	sceneManager->setWorld(gameWorld);
}

SpelMotor::~SpelMotor()
{
	shutdown();
}

void SpelMotor::run()
{
	timer->start();


}

void SpelMotor::shutdown()
{
	if (client)
	{
		client->disconnect();
	}

	if (server)
	{
		server->stop();
	}

	if (renderer)
	{
		InputManager::shutdown();
		renderer->close();
	}
}


SceneManager* SpelMotor::getSceneManager()
{
	if (sceneManager) return sceneManager.get();
	return nullptr;
}