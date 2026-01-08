#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/GameWorld.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
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
 * - W: Apply upward force to all circles
 * - A: Apply leftward force to all circles
 * - S: Apply downward force to all circles
 * - D: Apply rightward force to all circles
 * - ESC: Exit
 */
class PhysicsInputBehaviour: public Behaviour
{
   public:
	explicit PhysicsInputBehaviour(std::vector<GameObject*> circleGOs)
		: circleGOs(std::move(circleGOs)),
		  inputManager(nullptr),
		  physicsWorld(nullptr)
	{
	}

	~PhysicsInputBehaviour() override = default;

	void update(double deltaTime, const GameWorld& world) override
	{
		inputManager = world.input;
		physicsWorld = world.physics;

		const float forceMagnitude = 30000000.0f;
		Vector2 force = {0, 0};
		bool forceApplied = false;

		// Handle W key to apply upward force
		if ( inputManager->wasKeyPressed(KeyCode::W) )
		{
			force = {0, -forceMagnitude};
			forceApplied = true;
		}
		// Handle S key to apply downward force
		else if ( inputManager->wasKeyPressed(KeyCode::S) )
		{
			force = {0, forceMagnitude};
			forceApplied = true;
		}
		// Handle A key to apply leftward force
		else if ( inputManager->wasKeyPressed(KeyCode::A) )
		{
			force = {-forceMagnitude, 0};
			forceApplied = true;
		}
		// Handle D key to apply rightward force
		else if ( inputManager->wasKeyPressed(KeyCode::D) )
		{
			force = {forceMagnitude, 0};
			forceApplied = true;
		}

		// Apply force to all circles
		if ( forceApplied )
		{
			for ( auto* circleGO : circleGOs )
			{
				if ( circleGO != nullptr )
				{
					auto* rigidBody = circleGO->getComponent<RigidBody>();
					if ( rigidBody != nullptr )
					{
						physicsWorld->applyForce(rigidBody, force);
					}
				}
			}
			std::cout << "Force applied to all circles: " << force.x << ", "
					  << force.y << std::endl;
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
	std::vector<GameObject*> circleGOs;
};

void createCircle(std::unique_ptr<GameObject>& circle, float x, float y,
				  float radius, const Color& color, float density = 1.0f,
				  float restitution = 0.8f)
{
	circle = std::make_unique<GameObject>();
	circle->setName("Circle");
	circle->getTransform()->setPosition({x, y});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(radius).setColor(color);

	circle->addComponent<RigidBody>();
	auto* collider = circle->addComponent<Collider>();
	collider->setCircle(radius);
	collider->setDensity(density);
	collider->setRestitution(restitution);
}

void createRectangle(std::unique_ptr<GameObject>& rectangle, float x, float y,
					 const Vector2& size, const Color& color,
					 bool isStatic = true, float density = 1.0f,
					 float restitution = 0.6f)
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
	collider->setDensity(density);
	collider->setRestitution(restitution);
}

#undef main

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	ApplicationSpecifications spec = {};
	spec.networkingOptions.port = 8080;
	spec.networkingOptions.serverIP = "127.0.0.1";
	spec.networkingOptions.tickRate = 60;
	spec.renderSettings.renderBackend = RenderBackend::SDL;
	spec.renderSettings.windowOptions = {"Shape Sandbox", false, SCREEN_WIDTH,
										 SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time
	spec.engineSystem = EngineSystem::Client;

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto scene = std::make_unique<Scene>("PrototypeScene");

	// Create multiple bouncing balls with different properties
	std::vector<std::unique_ptr<GameObject>> circles;
	std::vector<GameObject*> circlePtrs;

	// Create balls at different positions with different bounciness
	auto& circle1 = circles.emplace_back();
	createCircle(circle1, 240.0f, 100.0f, 30.0f, Color::lightBlue(), 0.9f,
				 0.9f);	 // Very bouncy blue ball
	circlePtrs.push_back(circle1.get());

	auto& circle2 = circles.emplace_back();
	createCircle(circle2, 250.0f, 20.0f, 25.0f, Color::red(), 0.7f,
				 0.7f);	 // Moderately bouncy red ball
	circlePtrs.push_back(circle2.get());

	auto& circle3 = circles.emplace_back();
	createCircle(circle3, 260.0f, 200.0f, 35.0f, Color::green(), 0.85f,
				 0.85f);  // Bouncy green ball
	circlePtrs.push_back(circle3.get());

	// Create 10 very small circles for wow effect
	std::vector<std::pair<float, float>> smallCirclePositions = {
		{100.0f, 150.0f}, {150.0f, 80.0f}, {200.0f, 120.0f}, {300.0f, 60.0f},
		{350.0f, 140.0f}, {400.0f, 90.0f}, {120.0f, 250.0f}, {180.0f, 300.0f},
		{320.0f, 280.0f}, {380.0f, 320.0f}};

	std::vector<Color> smallCircleColors = {
		Color::yellow(),	  Color::pink(),		Color::purple(),
		Color::lightBlue(),	  Color::red(),			Color::green(),
		Color::lightYellow(), Color::lightPurple(), Color::orange(),
		Color::lightGreen()};

	for ( size_t i = 0; i < 10; ++i )
	{
		auto& smallCircle = circles.emplace_back();
		createCircle(smallCircle, smallCirclePositions[i].first,
					 smallCirclePositions[i].second, 8.0f, smallCircleColors[i],
					 0.8f, 0.9f);  // Small, bouncy circles
		circlePtrs.push_back(smallCircle.get());
	}

	for ( auto& circle : circles )
	{
		scene->addGameObject(std::move(circle));
	}

	// Create ground platform (static, bouncy)
	std::unique_ptr<GameObject> ground;
	createRectangle(ground, 250.0f, 450.0f, {500.0f, 50.0f}, Color::orange(),
					true, 0.6f, 0.6f);
	GameObject* groundGO = ground.get();
	scene->addGameObject(std::move(ground));

	// Create side walls for more interesting bouncing
	std::unique_ptr<GameObject> leftWall;
	createRectangle(leftWall, 25.0f, 250.0f, {50.0f, 400.0f}, Color::darkGray(),
					true, 0.5f, 0.5f);
	GameObject* leftWallGO = leftWall.get();
	scene->addGameObject(std::move(leftWall));

	std::unique_ptr<GameObject> rightWall;
	createRectangle(rightWall, 475.0f, 250.0f, {50.0f, 400.0f},
					Color::darkGray(), true, 0.5f, 0.5f);
	GameObject* rightWallGO = rightWall.get();
	scene->addGameObject(std::move(rightWall));

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	// Pass all circle pointers for WASD controls
	// Note: PhysicsInputBehaviour will get physics world from GameWorld in
	// update()
	inputHandler->addComponent<PhysicsInputBehaviour>(circlePtrs);

	scene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(scene));
	game->setApplicationSpecifications(spec);

	std::cout << "Physics Demo - Bouncing Balls\n";
	std::cout << "Watch the colorful balls bounce around!\n";
	std::cout << "Controls:\n";
	std::cout << "  W - Apply upward force to all balls\n";
	std::cout << "  A - Apply leftward force to all balls\n";
	std::cout << "  S - Apply downward force to all balls\n";
	std::cout << "  D - Apply rightward force to all balls\n";
	std::cout << "  ESC - Exit\n\n";

	return SpelMotorEntry::main(std::move(game));
}
