#include "Behaviour/DebugTimeControlBehaviour.h"

#include "Core/ApplicationClock.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <optional>

DebugTimeControlBehaviour::DebugTimeControlBehaviour(
	std::optional<KeyCode> pauseKey, std::optional<KeyCode> normalSpeedKey,
	std::optional<KeyCode> slowKey, std::optional<KeyCode> verySlowKey,
	std::optional<KeyCode> fastKey, std::optional<KeyCode> veryFastKey,
	bool printMenu)
	: inputManager(nullptr),
	  clock(nullptr),
	  menuPrinted(false),
	  movedToPersistentScene(false),
	  pauseKey(pauseKey),
	  normalSpeedKey(normalSpeedKey),
	  slowKey(slowKey),
	  verySlowKey(verySlowKey),
	  fastKey(fastKey),
	  veryFastKey(veryFastKey),
	  printMenu(printMenu)
{
}

void DebugTimeControlBehaviour::onAwake()
{
}

bool DebugTimeControlBehaviour::shouldRunWhenPaused() const
{
	return true;
}

void DebugTimeControlBehaviour::update(double deltaTime,
                                       const GameWorld& gameWorld)
{
	(void)deltaTime;

	inputManager = gameWorld.input;
	if (inputManager == nullptr)
	{
		return;
	}

	// Move to persistent scene on first update (if not already there)
	// This ensures debug controls persist across scene transitions
	if (!movedToPersistentScene &&
	   gameWorld.sceneManager != nullptr)
	{
		SceneManager* sceneManager = gameWorld.sceneManager;
		Scene* persistentScene = sceneManager->getOrCreatePersistentScene();
		GameObject* thisObject = getGameObject();

		if (thisObject != nullptr && persistentScene != nullptr)
		{
			const std::string objectName = thisObject->getName();

			// Check if object already exists in persistent scene
			if (persistentScene->getGameObject(objectName) == nullptr)
			{
				// Try to transfer from active scene if object is there
				Scene* activeScene = sceneManager->getActiveScene();
				if (activeScene != nullptr && activeScene != persistentScene &&
				    activeScene->getGameObject(objectName) != nullptr)
				{
					// Transfer from active scene to persistent scene
					sceneManager->transferGameObject(activeScene->getName(),
						persistentScene->getName(),
						objectName);
				}
				// Note: If object is in a non-active scene, it won't be
				// transferred automatically. Add debug controller directly to
				// persistent scene for best results.
			}
		}
		movedToPersistentScene =
			true; // Mark as attempted to avoid repeated checks
	}

	// Get clock from GameWorld
	if (gameWorld.clock != nullptr)
	{
		clock = gameWorld.clock;
	}

	// Print menu once when clock is available (if enabled)
	if (printMenu && clock != nullptr && !menuPrinted)
	{
		std::cout << "\n=== Debug Time Controls ===" << std::endl;
		if (pauseKey.has_value()) std::cout << "Pause key - Toggle pause" <<
		                          std::endl;
		if (normalSpeedKey.
			has_value()) std::cout << "Normal speed key - Normal speed (1.0x)"
			             << std::endl;
		if (slowKey.has_value()) std::cout << "Slow key - Slow motion (0.5x)" <<
		                         std::endl;
		if (verySlowKey.has_value())
			std::cout << "Very slow key - Very slow motion (0.10x)"
				<< std::endl;
		if (fastKey.has_value()) std::cout << "Fast key - Fast motion (2.0x)" <<
		                         std::endl;
		if (veryFastKey.
			has_value()) std::cout << "Very fast key - Very fast motion (4.0x)"
			             << std::endl;
		std::cout << "==========================\n" << std::endl;
		std::cout.flush();
		menuPrinted = true;
	}

	if (clock == nullptr)
	{
		return;
	}

	// Toggle pause using configured key
	if (pauseKey.has_value() && inputManager->wasKeyPressed(pauseKey.value()))
	{
		clock->togglePause();
		std::cout << "Simulation: "
			<< (clock->isPaused() ? "PAUSED" : "RUNNING") << std::endl;
	}

	// Time scale controls using configured keys (only if enabled)
	if (normalSpeedKey.has_value() &&
	    inputManager->wasKeyPressed(normalSpeedKey.value()))
	{
		clock->setTimeScale(1.0);
		std::cout << "Time scale: NORMAL (1.0x)" << std::endl;
	}
	else if (slowKey.has_value() &&
	         inputManager->wasKeyPressed(slowKey.value()))
	{
		clock->setTimeScale(0.5);
		std::cout << "Time scale: SLOW (0.5x)" << std::endl;
	}
	else if (verySlowKey.has_value() &&
	         inputManager->wasKeyPressed(verySlowKey.value()))
	{
		clock->setTimeScale(0.10);
		std::cout << "Time scale: VERY SLOW (0.10x)" << std::endl;
	}
	else if (fastKey.has_value() &&
	         inputManager->wasKeyPressed(fastKey.value()))
	{
		clock->setTimeScale(2.0);
		std::cout << "Time scale: FAST (2.0x)" << std::endl;
	}
	else if (veryFastKey.has_value() &&
	         inputManager->wasKeyPressed(veryFastKey.value()))
	{
		clock->setTimeScale(4.0);
		std::cout << "Time scale: VERY FAST (4.0x)" << std::endl;
	}
}