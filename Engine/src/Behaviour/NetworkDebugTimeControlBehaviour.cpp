#include "Behaviour/NetworkDebugTimeControlBehaviour.h"

#include "Core/ApplicationClock.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Networking/Messages/ConcreteMessages/ActionMessage.h"
#include "Networking/NetworkBuilder.h"
#include "Networking/Server/Server.h"
#include "Scene/SceneManager.h"

#include <iostream>

namespace
{
constexpr const char* kActionTogglePause = "DebugTime.TogglePause";
constexpr const char* kActionSetTimeScale = "DebugTime.SetTimeScale";
constexpr const char* kActionApplyTimeState = "DebugTime.ApplyTimeState";
constexpr double kNormalScale = 1.0;
constexpr double kSlowScale = 0.5;
constexpr double kVerySlowScale = 0.10;
constexpr double kFastScale = 2.0;
constexpr double kVeryFastScale = 4.0;
}  // namespace

NetworkDebugTimeControlBehaviour::NetworkDebugTimeControlBehaviour(
	std::optional<KeyCode> pauseKey, std::optional<KeyCode> normalSpeedKey,
	std::optional<KeyCode> slowKey, std::optional<KeyCode> verySlowKey,
	std::optional<KeyCode> fastKey, std::optional<KeyCode> veryFastKey,
	bool printMenu)
	: printMenu(printMenu),
	  pauseKey(pauseKey),
	  normalSpeedKey(normalSpeedKey),
	  slowKey(slowKey),
	  verySlowKey(verySlowKey),
	  fastKey(fastKey),
	  veryFastKey(veryFastKey)
{
}

void NetworkDebugTimeControlBehaviour::onStart()
{
	// Move to persistent scene so debug controls survive scene transitions.
	if ( gameWorld != nullptr && gameWorld->sceneManager != nullptr )
	{
		SceneManager* sceneManager = gameWorld->sceneManager;
		Scene* persistentScene = sceneManager->getOrCreatePersistentScene();
		GameObject* thisObject = getGameObject();

		if ( thisObject != nullptr && persistentScene != nullptr )
		{
			const std::string objectName = thisObject->getName();

			if ( persistentScene->getGameObject(objectName) == nullptr )
			{
				Scene* activeScene = sceneManager->getActiveScene();
				if ( activeScene != nullptr && activeScene != persistentScene &&
					 activeScene->getGameObject(objectName) != nullptr )
				{
					[[maybe_unused]] bool transferred =
						sceneManager->transferGameObject(
							activeScene->getName(), persistentScene->getName(),
							objectName);
				}
			}
		}
	}

	if ( gameWorld != nullptr )
	{
		clock = gameWorld->clock;
		if ( gameWorld->server != nullptr )
		{
			lastClientCount = gameWorld->server->getConnectedClientCount();
		}
	}

	maybePrintMenu();
}

void NetworkDebugTimeControlBehaviour::registerNetworkMethods(
	NetworkBuilder& builder)
{
	builder.command(kActionTogglePause,
					&NetworkDebugTimeControlBehaviour::commandTogglePause);
	builder.command(kActionSetTimeScale,
					&NetworkDebugTimeControlBehaviour::commandTimeScale);
	builder.command(kActionApplyTimeState,
					&NetworkDebugTimeControlBehaviour::applyTimeState);
}

bool NetworkDebugTimeControlBehaviour::shouldRunWhenPaused() const
{
	return true;
}

void NetworkDebugTimeControlBehaviour::update(double deltaTime,
											  const GameWorld& world)
{
	(void)deltaTime;

	if ( clock == nullptr && world.clock != nullptr )
	{
		clock = world.clock;
	}

	updateClientCount();
	maybePrintMenu();

	if ( !world.isClient() )
	{
		return;
	}

	inputManager = world.input;
	if ( inputManager == nullptr || clock == nullptr )
	{
		return;
	}

	if ( pauseKey.has_value() && inputManager->wasKeyPressed(pauseKey.value()) )
	{
		requestTogglePause();
	}

	if ( normalSpeedKey.has_value() &&
		 inputManager->wasKeyPressed(normalSpeedKey.value()) )
	{
		requestTimeScale(kNormalScale);
	}
	else if ( slowKey.has_value() &&
			  inputManager->wasKeyPressed(slowKey.value()) )
	{
		requestTimeScale(kSlowScale);
	}
	else if ( verySlowKey.has_value() &&
			  inputManager->wasKeyPressed(verySlowKey.value()) )
	{
		requestTimeScale(kVerySlowScale);
	}
	else if ( fastKey.has_value() &&
			  inputManager->wasKeyPressed(fastKey.value()) )
	{
		requestTimeScale(kFastScale);
	}
	else if ( veryFastKey.has_value() &&
			  inputManager->wasKeyPressed(veryFastKey.value()) )
	{
		requestTimeScale(kVeryFastScale);
	}
}

void NetworkDebugTimeControlBehaviour::requestTogglePause()
{
	if ( !isClient() )
	{
		return;
	}

	callCommand(kActionTogglePause);
}

void NetworkDebugTimeControlBehaviour::requestTimeScale(double scale)
{
	if ( !isClient() )
	{
		return;
	}

	callCommand(kActionSetTimeScale, scale);
}

void NetworkDebugTimeControlBehaviour::commandTogglePause()
{
	if ( !isServer() || gameWorld == nullptr || gameWorld->clock == nullptr )
	{
		return;
	}

	gameWorld->clock->togglePause();
	broadcastTimeState();

	std::cout << "Simulation: "
			  << (gameWorld->clock->isPaused() ? "PAUSED" : "RUNNING")
			  << std::endl;
}

void NetworkDebugTimeControlBehaviour::commandTimeScale(double scale)
{
	if ( !isServer() || gameWorld == nullptr || gameWorld->clock == nullptr )
	{
		return;
	}

	if ( scale <= 0.0 )
	{
		return;
	}

	gameWorld->clock->setTimeScale(scale);
	broadcastTimeState();

	std::cout << "Time scale: " << scale << "x" << std::endl;
}

void NetworkDebugTimeControlBehaviour::applyTimeState(double scale, bool paused)
{
	if ( gameWorld == nullptr || gameWorld->clock == nullptr )
	{
		return;
	}

	gameWorld->clock->setTimeScale(scale);
	if ( paused )
	{
		gameWorld->clock->pause();
	}
	else
	{
		gameWorld->clock->resume();
	}
}

void NetworkDebugTimeControlBehaviour::broadcastTimeState()
{
	if ( gameWorld == nullptr || gameWorld->server == nullptr || !identity )
	{
		return;
	}

	if ( gameWorld->clock == nullptr )
	{
		return;
	}

	WriteArchive archive;
	double scale = gameWorld->clock->getTimeScale();
	bool paused = gameWorld->clock->isPaused();
	archive.process(scale);
	archive.process(paused);

	ActionMessage message(componentNetworkId, identity->getNetId(),
						  kActionApplyTimeState, 0);
	message.setPayload(archive.getBytes());
	gameWorld->broadcastToClients(message);
}

void NetworkDebugTimeControlBehaviour::maybePrintMenu()
{
	if ( !printMenu || menuPrinted || gameWorld == nullptr )
	{
		return;
	}

	if ( !gameWorld->isClient() || clock == nullptr )
	{
		return;
	}

	std::cout << "\n=== Debug Time Controls (Networked) ===" << std::endl;
	if ( pauseKey.has_value() )
		std::cout << "Pause key - Toggle pause" << std::endl;
	if ( normalSpeedKey.has_value() )
		std::cout << "Normal speed key - Normal speed (1.0x)" << std::endl;
	if ( slowKey.has_value() )
		std::cout << "Slow key - Slow motion (0.5x)" << std::endl;
	if ( verySlowKey.has_value() )
		std::cout << "Very slow key - Very slow motion (0.10x)" << std::endl;
	if ( fastKey.has_value() )
		std::cout << "Fast key - Fast motion (2.0x)" << std::endl;
	if ( veryFastKey.has_value() )
		std::cout << "Very fast key - Very fast motion (4.0x)" << std::endl;
	std::cout << "===============================\n" << std::endl;
	std::cout.flush();
	menuPrinted = true;
}

void NetworkDebugTimeControlBehaviour::updateClientCount()
{
	if ( gameWorld == nullptr || gameWorld->server == nullptr )
	{
		return;
	}

	const int currentCount = gameWorld->server->getConnectedClientCount();
	if ( currentCount == lastClientCount )
	{
		return;
	}

	lastClientCount = currentCount;
	broadcastTimeState();
}
