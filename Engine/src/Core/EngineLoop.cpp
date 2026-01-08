#include "Core/EngineLoop.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Core/IEngineSystems.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Events/EventImplementations/ApplicationEvents.h"
#include "Networking/Client.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Server/Server.h"

#include <ostream>

EngineLoop::EngineLoop(std::unique_ptr<Game> game)
	: pendingSceneName()
	  , game(std::move(game))
	  , gameWorld(std::make_unique<GameWorld>())
	  , spawnManager(std::make_unique<NetworkSpawnManager>(gameWorld.get()))
	  , sceneManagerPtr(nullptr)
	  , clockFunction([]()
	  {
		  return 0.0;
	  })
{
	gameWorld->specs = this->game->getApplicationSpecifications();
}

EngineLoop::~EngineLoop() = default;

ApplicationSpecifications EngineLoop::getSpecifications() const
{
	return game->getApplicationSpecifications();
}

void EngineLoop::addSystem(std::unique_ptr<IEngineSystems> system)
{
	systems.push_back(std::move(system));
}

void EngineLoop::start()
{
	gameWorld->spawnManager = spawnManager.get();

	for (const auto& system : systems)
	{
		system->start(*gameWorld);
	}

	sceneManagerPtr = gameWorld->sceneManager;

	if (gameWorld->getDispatcher() != nullptr)
	{
		initializeCloseEvent(*gameWorld->getDispatcher());
	}

	initNetwork();

	if (!pendingSceneName.empty() && sceneManagerPtr)
	{
		sceneManagerPtr->setActiveScene(pendingSceneName);
	}
}

void EngineLoop::initNetwork() const
{
	if (hasFlag(game->getApplicationSpecifications().engineSystem , EngineSystem::NetClient))
	{
		sceneManagerPtr->configureNetworking(spawnManager.get());

		ServerConnectionInformation serverInfo{
			game->getApplicationSpecifications().networkingOptions.port,
			game->getApplicationSpecifications().networkingOptions.serverIP
		};

		if (gameWorld->getDispatcher() != nullptr)
		{
			gameWorld->client->connectToServer(serverInfo);
		}
	}

	if (hasFlag(game->getApplicationSpecifications().engineSystem , EngineSystem::NetServer))
	{
		sceneManagerPtr->configureNetworking(spawnManager.get());
	}
	if (gameWorld->server != nullptr)
	{
		NetworkSpawnManager* spawnMgr = spawnManager.get();

		gameWorld->server->setClientConnectedCallback(
			[spawnMgr](int clientId)
			{
				spawnMgr->syncExistingObjects(clientId);
			}
			);
	}
}

void EngineLoop::initializeCloseEvent(EventDispatcher& dispatcher)
{
	dispatcher.subscribe<WindowCloseEvent>([this](const WindowCloseEvent& e)
	{
		requestShutdown();
	});
}

void EngineLoop::update(const double deltaTime)
{
	for (const auto& system : systems)
	{
		system->update(deltaTime, *gameWorld);
	}
}

void EngineLoop::fixedUpdate(const double deltaTime)
{
	for (const auto& system : systems)
	{
		system->fixedUpdate(deltaTime, *gameWorld);
	}
}

void EngineLoop::shutdown()
{
	for (auto systemEntry = systems.rbegin(); systemEntry != systems.rend(); ++
	     systemEntry)
	{
		(*systemEntry)->shutdown(*gameWorld);
	}
	systems.clear();
}

bool EngineLoop::isShutdownRequested() const
{
	return shutdownRequested;
}

EngineLoop::ClockFunction EngineLoop::getClock()
{
	return clockFunction;
}

GameWorld* EngineLoop::getGameWorld()
{
	return gameWorld.get();
}

void EngineLoop::setGameWorld(std::unique_ptr<GameWorld> newGameWorld)
{
	this->gameWorld = std::move(newGameWorld);
}

SceneManager* EngineLoop::getSceneManager()
{
	return sceneManagerPtr;
}

Game* EngineLoop::getGame() const
{
	return game.get();
}

IBackendContext* EngineLoop::getBackendContext() const
{
	return backendContext.get();
}

const std::vector<std::unique_ptr<IEngineSystems>>&
EngineLoop::getSystems() const
{
	return systems;
}

void EngineLoop::setApplicationClock(ApplicationClock* clock)
{
	if (gameWorld)
	{
		gameWorld->clock = clock;
	}
}

void EngineLoop::setClockFunction(ClockFunction func)
{
	clockFunction = std::move(func);
}

void EngineLoop::setBackendContext(std::unique_ptr<IBackendContext> context)
{
	backendContext = std::move(context);
}

void EngineLoop::requestShutdown()
{
	shutdownRequested = true;
}