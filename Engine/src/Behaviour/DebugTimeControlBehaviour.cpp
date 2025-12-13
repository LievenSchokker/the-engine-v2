#include "Behaviour/DebugTimeControlBehaviour.h"

#include "Core/ApplicationClock.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"

#include <iostream>

DebugTimeControlBehaviour::DebugTimeControlBehaviour(
	KeyCode pauseKey, KeyCode normalSpeedKey, KeyCode slowKey,
	KeyCode verySlowKey, KeyCode fastKey, KeyCode veryFastKey, bool printMenu)
	: inputManager(nullptr),
	  clock(nullptr),
	  currentTimeScale(1.0),
	  menuPrinted(false),
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
	inputManager = InputManager::getInstance();
}

void DebugTimeControlBehaviour::update(float deltaTime, GameWorld* world)
{
	(void)deltaTime;

	if ( inputManager == nullptr )
	{
		return;
	}

	// Get clock from GameWorld
	if ( world != nullptr && world->clock != nullptr )
	{
		clock = world->clock;
	}

	// Print menu once when clock is available (if enabled)
	if ( printMenu && clock != nullptr && !menuPrinted )
	{
		std::cout << "\n=== Debug Time Controls ===" << std::endl;
		std::cout << "Pause key - Toggle pause" << std::endl;
		std::cout << "Normal speed key - Normal speed (1.0x)" << std::endl;
		std::cout << "Slow key - Slow motion (0.5x)" << std::endl;
		std::cout << "Very slow key - Very slow motion (0.25x)" << std::endl;
		std::cout << "Fast key - Fast motion (2.0x)" << std::endl;
		std::cout << "Very fast key - Very fast motion (4.0x)" << std::endl;
		std::cout << "ESC - Exit" << std::endl;
		std::cout << "==========================\n" << std::endl;
		std::cout.flush();
		menuPrinted = true;
	}

	if ( clock == nullptr )
	{
		return;
	}

	// Toggle pause using configured key
	if ( inputManager->wasKeyPressed(pauseKey) )
	{
		clock->togglePause();
		std::cout << "Simulation: "
				  << (clock->isPaused() ? "PAUSED" : "RUNNING") << std::endl;
	}

	// Time scale controls using configured keys
	if ( inputManager->wasKeyPressed(normalSpeedKey) )
	{
		clock->setTimeScale(1.0);
		currentTimeScale = 1.0;
		std::cout << "Time scale: NORMAL (1.0x)" << std::endl;
	}
	else if ( inputManager->wasKeyPressed(slowKey) )
	{
		clock->setTimeScale(0.5);
		currentTimeScale = 0.5;
		std::cout << "Time scale: SLOW (0.5x)" << std::endl;
	}
	else if ( inputManager->wasKeyPressed(verySlowKey) )
	{
		clock->setTimeScale(0.25);
		currentTimeScale = 0.25;
		std::cout << "Time scale: VERY SLOW (0.25x)" << std::endl;
	}
	else if ( inputManager->wasKeyPressed(fastKey) )
	{
		clock->setTimeScale(2.0);
		currentTimeScale = 2.0;
		std::cout << "Time scale: FAST (2.0x)" << std::endl;
	}
	else if ( inputManager->wasKeyPressed(veryFastKey) )
	{
		clock->setTimeScale(4.0);
		currentTimeScale = 4.0;
		std::cout << "Time scale: VERY FAST (4.0x)" << std::endl;
	}

	// Exit (always ESC, not configurable as it's a system-level control)
	if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) )
	{
		inputManager->signalQuit();
	}
}
