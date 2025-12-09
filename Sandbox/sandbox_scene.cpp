#include "Behaviour/Behaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/RenderSystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 700

/**
 * @brief Behavior class that handles input for the sandbox scene.
 *
 * Handles:
 * - SPACE key: Toggles clear color between dark green and dark purple
 * - Mouse wheel Y: Rotates the YellowRectangle
 * - Mouse wheel X: Moves and scales the BlueCircle
 */
class SandboxInputBehaviour: public Behaviour
{
   public:
	explicit SandboxInputBehaviour(Scene* scene)
		: scene(scene), inputManager(nullptr), clearColor(Color::darkGray())
	{
	}

	~SandboxInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();
	}

	void update(float deltaTime, GameWorld* world) override
	{
		(void)deltaTime;

		if ( inputManager == nullptr || scene == nullptr )
		{
			return;
		}

		// Handle SPACE key to toggle clear color
		if ( inputManager->wasKeyPressed(KeyCode::SPACE) )
		{
			clearColor = (clearColor == Color::darkGreen())
							 ? Color::darkPurple()
							 : Color::darkGreen();
			// Try to set clear color through RenderSystem if available
			if ( world != nullptr && world->render != nullptr )
			{
				world->render->setClearColor(clearColor);
			}
		}

		// Handle mouse wheel Y to rotate rectangle
		if ( inputManager->wheelDeltaY() != 0 )
		{
			GameObject* rectangle = scene->getGameObject("YellowRectangle");
			if ( rectangle != nullptr )
			{
				double currentRotation =
					rectangle->getTransform()->getRotationAngle();
				rectangle->getTransform()->setRotationAngle(
					currentRotation + inputManager->wheelDeltaY());
			}
		}

		// Handle mouse wheel X to move and scale circle
		if ( inputManager->wheelDeltaX() != 0 )
		{
			GameObject* circle = scene->getGameObject("BlueCircle");
			if ( circle != nullptr )
			{
				Vector2 currentPosition = circle->getTransform()->getPosition();
				circle->getTransform()->setPosition(
					{currentPosition.x +
						 static_cast<float>(inputManager->wheelDeltaX()) *
							 10.0f,
					 currentPosition.y});
				Vector2 currentScale = circle->getTransform()->getScale();
				circle->getTransform()->setScale(
					{currentScale.x +
						 static_cast<float>(inputManager->wheelDeltaX()) * 0.1f,
					 currentScale.y +
						 static_cast<float>(inputManager->wheelDeltaX()) *
							 0.1f});
			}
		}
	}

   private:
	Scene* scene;
	InputManager* inputManager;
	Color clearColor;
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
	spec.renderBackend = RenderBackend::SDL;
	spec.windowOptions = {"Shape Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto prototypeScene = std::make_unique<Scene>("PrototypeScene");

	auto circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	auto* circleRenderer = circle->addComponent<ShapeRenderer>();
	if ( circleRenderer == nullptr )
	{
		std::cerr << "[Sandbox] Failed to add ShapeRenderer to BlueCircle\n";
		return 1;
	}
	circleRenderer->setCircle(50.0).setColor(Color::lightBlue());

	auto rectangle = std::make_unique<GameObject>();
	rectangle->setName("YellowRectangle");
	rectangle->getTransform()->setPosition({320.0, 240.0});
	rectangle->getTransform()->setRotationAngle(25.0);
	rectangle->getTransform()->setScale({1.0, 1.0});
	auto* rectangleRenderer = rectangle->addComponent<ShapeRenderer>();
	if ( rectangleRenderer == nullptr )
	{
		std::cerr
			<< "[Sandbox] Failed to add ShapeRenderer to YellowRectangle\n";
		return 1;
	}
	rectangleRenderer->setRectangle({140.0, 80.0}).setColor(Color::lightRed());

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	// Add the behavior with a reference to the scene
	inputHandler->addComponent<SandboxInputBehaviour>(prototypeScene.get());

	prototypeScene->addGameObject(std::move(circle));
	prototypeScene->addGameObject(std::move(rectangle));
	prototypeScene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(prototypeScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
