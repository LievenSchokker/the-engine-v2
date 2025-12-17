#include "Behaviour/PhysicsUpdateBehaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/ApplicationSpecifications.h"
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
#include <vector>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

/**
 * @brief Behavior class that handles physics demo input.
 *
 * Handles:
 * - W: Destroy circle body
 * - SPACE: Apply force to circle
 * - ESC: Exit
 */
class PhysicsInputBehaviour: public Behaviour
{
   public:
	PhysicsInputBehaviour(IPhysicsWorld* physicsWorld, GameObject* circleGO)
		: inputManager(nullptr),
		  physicsWorld(physicsWorld),
		  circleGO(circleGO)
	{
	}

	~PhysicsInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();
	}

	void update(float deltaTime, GameWorld* world) override
	{
		(void)deltaTime;
		(void)world;

		if ( inputManager == nullptr || physicsWorld == nullptr ||
			 circleGO == nullptr )
		{
			return;
		}

		// Handle W key to destroy rectangle body
		if ( inputManager->wasKeyPressed(KeyCode::W) )
		{
			physicsWorld->destroyBody(circleGO->getComponent<RigidBody>());
			std::cout << "Body destroyed for " << circleGO->getName()
					  << std::endl;
		}

		// Handle SPACE key to apply force
		if ( inputManager->wasKeyPressed(KeyCode::SPACE) )
		{
			const Vector2 force = {0, -30000000};
			physicsWorld->applyForce(circleGO->getComponent<RigidBody>(),
									 force);
			std::cout << "Force applied to " << circleGO->getName()
					  << " with force: " << force.x << ", " << force.y
					  << std::endl;
		}

		// Handle ESC key for exit
		if ( inputManager->wasKeyPressed(KeyCode::ESCAPE) )
		{
			inputManager->signalQuit();
			std::cout << "Exiting..." << std::endl;
		}
	}

   private:
	InputManager* inputManager;
	IPhysicsWorld* physicsWorld;
	GameObject* circleGO;
};

void createCircle(std::unique_ptr<GameObject>& circle, float x, float y,
				  float radius, const Color& color, float restitution = 0.8f)
{
	circle = std::make_unique<GameObject>();
	circle->setName("Circle");
	circle->getTransform()->setPosition({x, y});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(radius).setColor(color);

	circle->addComponent<RigidBody>();
	auto* collider = circle->addComponent<Collider>();
	collider->setCircle(radius);
	collider->restitution = restitution;
}

void createRectangle(std::unique_ptr<GameObject>& rectangle, float x, float y,
					 const Vector2& size, const Color& color,
					 bool isStatic = true, float restitution = 0.5f)
{
	rectangle = std::make_unique<GameObject>();
	rectangle->setName("Rectangle");
	rectangle->getTransform()->setPosition({x, y});
	rectangle->addComponent<ShapeRenderer>()->setRectangle(size).setColor(
		color);

	auto rb = rectangle->addComponent<RigidBody>();
	if ( isStatic )
	{
		rb->makeStatic();
	}
	auto* collider = rectangle->addComponent<Collider>();
	collider->setRectangle(size);
	collider->restitution = restitution;
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

	// Create multiple bouncing balls with different properties
	std::vector<std::unique_ptr<GameObject>> circles;
	std::vector<GameObject*> circleGOs;

	// Create balls at different positions with different bounciness
	createCircle(circles.emplace_back(), 240.0f, 100.0f, 30.0f,
				 Color::lightBlue(),
				 0.9f);	 // Very bouncy blue ball
	createCircle(circles.emplace_back(), 250.0f, 20.0f, 25.0f, Color::red(),
				 0.7f);	 // Moderately bouncy red ball
	createCircle(circles.emplace_back(), 260.0f, 200.0f, 35.0f, Color::green(),
				 0.85f);  // Bouncy green ball

	for ( auto& circle : circles )
	{
		circleGOs.push_back(circle.get());
		scene->addGameObject(std::move(circle));
	}

	// Create ground platform (static, bouncy)
	std::unique_ptr<GameObject> ground;
	createRectangle(ground, 250.0f, 450.0f, {500.0f, 50.0f}, Color::orange(),
					true, 0.6f);
	GameObject* groundGO = ground.get();
	scene->addGameObject(std::move(ground));

	// Create side walls for more interesting bouncing
	std::unique_ptr<GameObject> leftWall;
	createRectangle(leftWall, 25.0f, 250.0f, {50.0f, 400.0f}, Color::darkGray(),
					true, 0.5f);
	GameObject* leftWallGO = leftWall.get();
	scene->addGameObject(std::move(leftWall));

	std::unique_ptr<GameObject> rightWall;
	createRectangle(rightWall, 475.0f, 250.0f, {50.0f, 400.0f},
					Color::darkGray(), true, 0.5f);
	GameObject* rightWallGO = rightWall.get();
	scene->addGameObject(std::move(rightWall));

	// --- Physics World ---
	std::unique_ptr<IPhysicsWorld> physicsWorld =
		std::make_unique<Box2DPhysicsWorld>();
	physicsWorld->start();

	// Register all GameObjects with physics world
	for ( GameObject* circleGO : circleGOs )
	{
		physicsWorld->createBody(circleGO->getComponent<RigidBody>());
	}
	physicsWorld->createBody(groundGO->getComponent<RigidBody>());
	physicsWorld->createBody(leftWallGO->getComponent<RigidBody>());
	physicsWorld->createBody(rightWallGO->getComponent<RigidBody>());

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	// Use first circle for input controls (W to destroy, SPACE to apply force)
	inputHandler->addComponent<PhysicsInputBehaviour>(physicsWorld.get(),
													  circleGOs[0]);

	// Create a GameObject to update physics world in fixed update loop
	auto physicsUpdater = std::make_unique<GameObject>();
	physicsUpdater->setName("PhysicsUpdater");
	physicsUpdater->addComponent<PhysicsUpdateBehaviour>(physicsWorld.get());

	scene->addGameObject(std::move(inputHandler));
	scene->addGameObject(std::move(physicsUpdater));

	game->addScene(std::move(scene));
	game->setApplicationSpecifications(spec);

	std::cout << "Physics Demo - Bouncing Balls\n";
	std::cout << "Watch the colorful balls bounce around!\n";
	std::cout << "Controls:\n";
	std::cout << "  W - Destroy first ball\n";
	std::cout << "  SPACE - Apply upward force to first ball\n";
	std::cout << "  ESC - Exit\n\n";

	return SpelMotorEntry::main(std::move(game));
}
