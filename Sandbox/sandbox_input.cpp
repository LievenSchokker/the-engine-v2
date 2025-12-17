#include "Behaviour/Behaviour.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Scene/Scene.h"

#include <iostream>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

/**
 * @brief Behavior class that handles input testing and demonstration.
 *
 * Tests various input scenarios and prints results to console.
 */
class InputTestBehaviour: public Behaviour
{
   public:
	InputTestBehaviour() : inputManager(nullptr)
	{
	}

	~InputTestBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();

		std::cout << "\nTry the following:\n" << std::endl;
		std::cout << "- Press W and A to see if they are currently held"
				  << std::endl;
		std::cout
			<< "- Press the left mouse button and the right mouse button "
			   "to see if "
			   "they are currently held and if the right mouse button was "
			   "pressed"
			<< std::endl;
		std::cout << "- Move the mouse to see if it moved this frame and is "
					 "close "
					 "to the top left corner (x < 20 && y < 20)"
				  << std::endl;
		std::cout << "- Scroll the mouse wheel to see if the wheel delta is "
					 "not 0"
				  << std::endl;
		std::cout << "- Press the escape key to quit" << std::endl;
	}

	void update(double deltaTime, const GameWorld& world) override
	{
		(void)deltaTime;
		(void)world;

		if ( inputManager == nullptr )
		{
			return;
		}

		// ================================ GAME CODE CHECK
		// ================================
		{
			if ( inputManager->isKeyDown(KeyCode::W) &&
				 inputManager->isKeyDown(KeyCode::A) )
			{
				std::cout << "W and A are currently held (game code check)"
						  << std::endl;
			}
			// =================================================================================
			if ( inputManager->isMouseDown(MouseButton::LEFT) &&
				 inputManager->wasMousePressed(MouseButton::RIGHT) )
			{
				std::cout << "Left mouse button is currently held and right "
							 "mouse button was pressed (game code check)"
						  << std::endl;
			}
			// =================================================================================
			if ( inputManager->mouseMovedThisFrame() &&
				 inputManager->mouseX() < 20 && inputManager->mouseY() < 20 )
			{
				std::cout << "Mouse moved this frame and is close to the top "
							 "left corner (game code check)"
						  << std::endl;
			}
			// =================================================================================
			if ( inputManager->wheelDeltaX() != 0 ||
				 inputManager->wheelDeltaY() != 0 )
			{
				std::cout << "Mouse wheel delta: ("
						  << inputManager->wheelDeltaX() << ", "
						  << inputManager->wheelDeltaY() << ")" << std::endl;
			}
			// =================================================================================
			if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) )
			{
				std::cout << "Escape key was pressed (game code check)"
						  << std::endl;
				inputManager->signalQuit();
			}
		}
	}

   private:
	InputManager* inputManager;
};

#undef main

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.mode = EngineMode::CLIENT;
	spec.networkingOptions.tickRate = 60;
    spec.engineSystem = EngineSystem::Client;
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"Basic C++ SDL project", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create a minimal scene for the input test
	auto inputScene = std::make_unique<Scene>("InputScene");

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	inputHandler->addComponent<InputTestBehaviour>();

	inputScene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(inputScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
