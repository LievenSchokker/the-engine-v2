<<<<<<< HEAD
=======
#include "Game.h"
>>>>>>> origin/development
#include "Core/SpelMotor.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
<<<<<<< HEAD
#include "Physics/Box2D/Box2DPhysicsWorld.h"
=======
>>>>>>> origin/development
#include "Rendering/SDL/SDLRenderer.h"
#include "Networking/Server/Server.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

<<<<<<< HEAD
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Scene/SceneManager.h"
#include "Networking/Messages/Concretes/WelcomeMessage.h"
#include "Networking/Server/StateSyncSystem.h"

SpelMotor::SpelMotor(ApplicationSpecifications applicationSpecifications)
	: specifications(applicationSpecifications)
	  , tickRate(applicationSpecifications.networkingOptions.tickRate)
	  , physicsWorld(std::make_unique<Box2DPhysicsWorld>(tickRate))
	  , sceneManager(std::make_unique<SceneManager>())
{
	const auto& netOpts = specifications.networkingOptions;

	if (netOpts.mode == EngineMode::CLIENT)
	{
		if (specifications.renderBackend == RenderBackend::SDL)
		{
			SdlContext context = SdlContext();
			timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []()
			{
				return SDL_GetTicks() / 1000.0;
			});
			renderer = std::make_unique<SDLRenderer>(context);
		}

		client = std::make_unique<Client>(std::make_unique<TransportGNS>());
		gameWorld.renderer = renderer.get();
		gameWorld.client = client.get();
	}
	else
	{
		timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []()
		{
			using namespace std::chrono;
			return duration<double>(steady_clock::now().time_since_epoch()).
				count();
		});

		ServerConnectionInformation serverInfo;
		serverInfo.port = netOpts.port;
		server = std::make_unique<Server>(serverInfo,
		                                  std::make_unique<TransportGNS>());

		gameWorld.server = server.get();
=======
#include "Core/IEngineLoop.h"
#include "Core/EngineLoopFactory.h"
#include "Scene/SceneManager.h"

SpelMotor::SpelMotor(std::unique_ptr<Game> game)
	:
	running(false),
	specifications(game->getApplicationSpecifications()),
	coreSystemLoop(
		EngineLoopFactory::createEngineLoop(std::move(game))),
	coreClock(std::make_unique<ApplicationClock>(coreSystemLoop->getClock(),
	                                             specifications.
	                                             networkingOptions.tickRate,
	                                             specifications.
	                                             maxFrameTime))
{
	if (coreSystemLoop == nullptr)
	{
		throw std::runtime_error(
			"Core System Loop is null double check your applicationSpecifications.");
>>>>>>> origin/development
	}

	gameWorld.sceneManager = sceneManager.get();
	gameWorld.physics = physicsWorld.get();
	gameWorld.input = InputManager::getInstance();

	sceneManager->setWorld(&gameWorld);
}

SpelMotor::~SpelMotor()
<<<<<<< HEAD
{
	shutdown();
}

void SpelMotor::run()
{
	timer->start();
	physicsWorld->start();

	initializeNetworking();

	if (specifications.networkingOptions.mode == EngineMode::CLIENT)
	{
		runClient();
	}
	else
	{
		runServer();
	}
}

void SpelMotor::runClient()
=======
{
	shutdown();
}

void SpelMotor::start()
{
	coreClock->start();
	coreSystemLoop->start();
	run();
}

void SpelMotor::run()
>>>>>>> origin/development
{
	renderer->open(specifications.windowOptions);

	ServerConnectionInformation serverInfo;
	serverInfo.ip = specifications.networkingOptions.serverIP;
	serverInfo.port = specifications.networkingOptions.port;

	if (!client->connectToServer(serverInfo))
	{
		return;
	}

	startNetworkThread();
	RenderQueue renderQueue;
	running = true;

	while (running)
	{
<<<<<<< HEAD
		timer->tick();
		client->poll();
		InputManager::getInstance()->update();

		while (timer->shouldFixedUpdate())
		{
			if (sceneManager != nullptr)
			{
				sceneManager->update(timer->getDeltaTime());
				physicsWorld->update();
			}

			timer->consumeFixedUpdate();
		}
		sceneManager->buildRenderQueue(renderQueue);
		renderer->render(renderQueue);
		if (InputManager::getInstance()->quitRequested())
		{
			shutdown();
		}
	}
}

void SpelMotor::initializeNetworking()
{
	Scene* activeScene = sceneManager->getActiveScene();
	if (!activeScene)
	{
		return;
	}

	identityRegistry = std::make_unique<NetworkIdentityRegistry>();

	if (server)
	{
		spawnManager = std::make_unique<NetworkSpawnManager>(
			server.get(), activeScene, identityRegistry.get());

		gameWorld.spawnManager = spawnManager.get();

		server->setClientConnectedCallback([this](int clientId)
		{
			WelcomeMessage welcome(clientId);
			server->sendMessage(clientId, welcome);

			spawnManager->syncExistingObjects(clientId);
			Vector2 spawnPos{350.0f + (clientId * 60.0f), 350.0f};
			spawnManager->spawnPlayer(clientId, spawnPos);
		});

		server->setClientDisconnectedCallback([this](int clientId)
		{
			spawnManager->despawnClientObjects(clientId);
		});

		auto dispatcher =
			spelmotor_networking::MessageDispatcherFactory::createServerDispatcher(
				gameWorld,
				*spawnManager,
				server->getNetworkContext(),
				*identityRegistry);
		server->injectMessageDispatcher(std::move(dispatcher));
	}
	else if (client)
	{
		spawnManager = std::make_unique<NetworkSpawnManager>(
			server.get(), activeScene, identityRegistry.get());
		gameWorld.spawnManager = spawnManager.get();

		auto dispatcher =
			spelmotor_networking::MessageDispatcherFactory::createClientDispatcher(
				gameWorld,
				*spawnManager,
				client->getNetworkContext(),
				*identityRegistry);
		client->injectMessageDispatcher(std::move(dispatcher));
	}
}

void SpelMotor::runServer()
{
	if (server->start() != ServerStatus::Running)
	{
		return;
	}

	StateSyncSystem stateSync(server.get(), identityRegistry.get());

	running = true;
	uint32_t currentTick = 0;

	while (running)
	{
		timer->tick();
		server->update();
		while (timer->shouldFixedUpdate())
		{
			currentTick++;

			if (sceneManager != nullptr)
			{
				sceneManager->update(timer->getDeltaTime());
			}


			stateSync.tick(currentTick);
			physicsWorld->update();
			timer->consumeFixedUpdate();
		}
	}
}



void SpelMotor::shutdown()
{
	if (!networkRunning.exchange(false))
	{
		return;
	}

	stopNetworkThread();

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

	physicsWorld->shutdown();
}


SceneManager* SpelMotor::getSceneManager()
{
	if (sceneManager) return sceneManager.get();
	return nullptr;
=======
		coreClock->tick();

		while (coreClock->shouldFixedUpdate())
		{
			coreSystemLoop->fixedUpdate(coreClock->getDeltaTime());
			coreClock->consumeFixedUpdate();
		}

		coreSystemLoop->update(coreClock->getDeltaTime());
	}
}

void SpelMotor::shutdown() const
{
	coreSystemLoop->shutdown();
>>>>>>> origin/development
}