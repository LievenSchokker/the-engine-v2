#include "Behaviour/Behaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "../Engine/inc/Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

/**
 * @brief Behavior class that handles physics demo input.
 *
 * Handles:
 * - W: Destroy rectangle body
 * - SPACE: Apply force to rectangle
 * - ESC: Exit
 */
class PhysicsInputBehaviour: public Behaviour
{
   public:
	PhysicsInputBehaviour(IPhysicsWorld* physicsWorld, GameObject* rectangleGO)
		: inputManager(nullptr),
		  physicsWorld(physicsWorld),
		  rectangleGO(rectangleGO)
	{
	}

	~PhysicsInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();
	}

	void update(double deltaTime, const GameWorld& world) override
	{
		(void)deltaTime;
		(void)world;

		if ( inputManager == nullptr || physicsWorld == nullptr ||
			 rectangleGO == nullptr )
		{
			return;
		}

		// Handle W key to destroy rectangle body
		if ( inputManager->wasKeyPressed(KeyCode::W) )
		{
			physicsWorld->destroyBody(rectangleGO->getComponent<RigidBody>());
		}

		// Handle SPACE key to apply force
		if ( inputManager->wasKeyPressed(KeyCode::SPACE) )
		{
			const Vector2 force = {0, 100};
			physicsWorld->applyForce(rectangleGO->getComponent<RigidBody>(),
									 force);
		}

		// Handle ESC key for exit
		if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) )
		{
			inputManager->signalQuit();
		}
	}

   private:
	InputManager* inputManager;
	IPhysicsWorld* physicsWorld;
	GameObject* rectangleGO;
};

void createCircle(std::unique_ptr<GameObject>& circle)
{
	circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50).setColor(
		Color::lightBlue());

	circle->addComponent<RigidBody>();
	circle->addComponent<Collider>()->setCircle(50);
}

void createRectangle(std::unique_ptr<GameObject>& rectangle)
{
	rectangle = std::make_unique<GameObject>();
	rectangle->setName("OrangeSquare");
	rectangle->getTransform()->setPosition({30.0, 450.0});
	rectangle->addComponent<ShapeRenderer>()->setRectangle({400, 50}).setColor(
		Color::orange());

	auto rb = rectangle->addComponent<RigidBody>();
	rb->makeStatic();
	rectangle->addComponent<Collider>()->setRectangle({400, 50});
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
	spec.windowOptions = {"Shape Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto scene = std::make_unique<Scene>("PrototypeScene");

	std::unique_ptr<GameObject> circle;
	std::unique_ptr<GameObject> rectangle;

	createCircle(circle);
	createRectangle(rectangle);

	// Store references before moving
	GameObject* circleGO = circle.get();
	GameObject* rectangleGO = rectangle.get();

	scene->addGameObject(std::move(circle));
	scene->addGameObject(std::move(rectangle));

	// --- Physics World ---
	std::unique_ptr<IPhysicsWorld> physicsWorld =
		std::make_unique<Box2DPhysicsWorld>();
	physicsWorld->initialize();

	// Register GameObjects with physics world
	physicsWorld->createBody(circleGO->getComponent<RigidBody>());
	physicsWorld->createBody(rectangleGO->getComponent<RigidBody>());

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	inputHandler->addComponent<PhysicsInputBehaviour>(physicsWorld.get(),
													  rectangleGO);

	scene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(scene));
	game->setApplicationSpecifications(spec);

	// Note: Physics world needs to be updated, but since we're using the
	// engine's fixed update system, we need to integrate physics updates.
	// For now, this is a simplified version. In a full implementation,
	// physics would be integrated into the engine's fixed update loop.

	std::cout << "Physics Demo\n";
	std::cout << "Controls:\n";
	std::cout << "  W - Destroy rectangle body\n";
	std::cout << "  SPACE - Apply upward force to rectangle\n";
	std::cout << "  ESC - Exit\n\n";

	// Store physics world in a way that can be accessed during updates
	// This is a workaround - ideally physics would be integrated into GameWorld
	// For now, we'll need to manually update physics in a behavior or extend
	// the engine loop

	return SpelMotorEntry::main(std::move(game));
}
