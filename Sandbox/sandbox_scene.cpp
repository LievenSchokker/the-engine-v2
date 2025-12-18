#include "Core/Options/ApplicationSpecifications.h"
#include "Behaviour/Behaviour.h"
#include "Behaviours/SimpleMoveBehaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
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
class SandboxInputBehaviour final: public Behaviour
{
   public:
	explicit SandboxInputBehaviour(Scene* scene)
		: scene(scene), inputManager(nullptr)
	{
	}

	~SandboxInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();
	}

	void update(double deltaTime, const GameWorld& world) override
	{
		(void)deltaTime;

		if ( inputManager == nullptr || scene == nullptr )
		{
			return;
		}
		if ( inputManager->wasKeyPressed(KeyCode::D) )
		{
			auto circle = scene->getGameObject("BlueCircle");
			auto component = circle->getComponent<SimpleMoveBehaviour>();
			component->setEnabled(!component->getIsEnabled());
		}

		// Handle SPACE key to toggle clear color
		if ( inputManager->wasKeyPressed(KeyCode::SPACE) )
		{
			auto w = world;
			Color clearColor = world.render->getClearColor();
			clearColor = (clearColor == Color::darkGreen())
							 ? Color::darkPurple()
							 : Color::darkGreen();

			// Try to set clear color through RenderSystem if available
			if ( world.render != nullptr )
			{
				world.render->setClearColor(clearColor);
			}
		}

		if ( inputManager->wasKeyPressed(KeyCode::ENTER) )
		{
			SceneManager* sm = world.sceneManager;
			Scene* active = sm->getActiveScene();

			if ( active->getName() == "PrototypeScene" )
				sm->setActiveScene("SecondScene");
			else
			{
				bool hardReset = false;
				if ( hardReset )
				{
					sm->removeScene("PrototypeScene");
					auto newScene = createPrototypeScene();
					sm->addScene(std::move(newScene));
					sm->setActiveScene("PrototypeScene");
				}
				else
				{
					sm->setActiveScene("PrototypeScene");
				}
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

	std::unique_ptr<Scene> createPrototypeScene()
	{
		auto scene = std::make_unique<Scene>("PrototypeScene");

		auto circle = std::make_unique<GameObject>();
		circle->setName("BlueCircle");
		circle->getTransform()->setPosition({150.0, 140.0});
		circle->getTransform()->setScale({1.0, 1.0});
		circle->addComponent<ShapeRenderer>()->setCircle(50.0).setColor(
			Color::lightBlue());
		circle->addComponent<SimpleMoveBehaviour>();

		auto rectangle = std::make_unique<GameObject>();
		rectangle->setName("YellowRectangle");
		rectangle->getTransform()->setPosition({320.0, 240.0});
		rectangle->getTransform()->setRotationAngle(25.0);
		rectangle->getTransform()->setScale({1.0, 1.0});
		rectangle->addComponent<ShapeRenderer>()
			->setRectangle({140.0, 80.0})
			.setColor(Color::lightRed());

		scene->addGameObject(std::move(circle));
		scene->addGameObject(std::move(rectangle));

		return scene;
	}

   private:
	Scene* scene;
	InputManager* inputManager;
};

//////////////////////////////
/// Create Scenes
//////////////////////////////

std::unique_ptr<Scene> createPrototypeScene()
{
	auto scene = std::make_unique<Scene>("PrototypeScene");

	auto circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50.0).setColor(
		Color::lightBlue());
	circle->addComponent<SimpleMoveBehaviour>();

	auto rectangle = std::make_unique<GameObject>();
	rectangle->setName("YellowRectangle");
	rectangle->getTransform()->setPosition({320.0, 240.0});
	rectangle->getTransform()->setRotationAngle(25.0);
	rectangle->getTransform()->setScale({1.0, 1.0});
	rectangle->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::lightRed());

	scene->addGameObject(std::move(circle));
	scene->addGameObject(std::move(rectangle));

	return scene;
}

std::unique_ptr<Scene> createSecondScene()
{
	auto scene = std::make_unique<Scene>("SecondScene");

	auto rectA = std::make_unique<GameObject>();
	rectA->setName("RectA");
	rectA->getTransform()->setPosition({200.0f, 200.0f});
	rectA->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::yellow());
	scene->addGameObject(std::move(rectA));

	auto rectB = std::make_unique<GameObject>();
	rectB->setName("RectB");
	rectB->getTransform()->setPosition({500.0f, 200.0f});
	rectB->addComponent<ShapeRenderer>()
		->setRectangle({140.0, 80.0})
		.setColor(Color::green());
	scene->addGameObject(std::move(rectB));

	// auto handler = std::make_unique<GameObject>();
	// handler->setName("InputHandler");
	// handler->addComponent<SandboxInputBehaviour>(scene.get());
	// scene->addGameObject(std::move(handler));

	return scene;
}

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
	spec.engineSystem = EngineSystem::Client;
	spec.windowOptions = {"Shape Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto prototypeScene = createPrototypeScene();
	auto secondScene = createSecondScene();

	// Attach behavior
	auto goInputHandler = std::make_unique<GameObject>();
	goInputHandler->setName("InputHandler");
	goInputHandler->addComponent<SandboxInputBehaviour>(prototypeScene.get());
	prototypeScene->addGameObject(std::move(goInputHandler));
	game->addScene(std::move(prototypeScene));

	// Attach behavior
	auto goInputHandler2 = std::make_unique<GameObject>();
	goInputHandler2->setName("InputHandler");
	goInputHandler2->addComponent<SandboxInputBehaviour>(secondScene.get());
	secondScene->addGameObject(std::move(goInputHandler2));
	game->addScene(std::move(secondScene));

	// game->addScene(createSecondScene());
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}