#include "Behaviour/DebugTimeControlBehaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "EntryPoint.h"
#include "Game.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

/**
 * @brief A simple behavior that moves an object to demonstrate time scaling.
 */
class MovingObjectBehaviour: public Behaviour
{
   public:
	MovingObjectBehaviour(float speed, float boundary)
		: speed(speed), boundary(boundary), direction(1.0f)
	{
	}

	void update(double deltaTime, const GameWorld& gameWorld) override
	{
		Transform* transform = getGameObject()->getTransform();
		if ( transform == nullptr )
		{
			return;
		}

		Vector2 pos = transform->getPosition();

		// Move horizontally
		pos.x += speed * direction * deltaTime;

		// Bounce at boundaries
		if ( pos.x >= boundary || pos.x <= 0 )
		{
			direction *= -1.0f;
			pos.x = (pos.x >= boundary) ? boundary : 0.0f;
		}

		transform->setPosition(pos);
	}

   private:
	float speed;
	float boundary;
	float direction;
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
    spec.engineSystem = EngineSystem::Client;
	spec.windowOptions = {"Debug Time Controls Demo", SCREEN_WIDTH,
						  SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;

	std::unique_ptr<Game> game = std::make_unique<Game>();

	// Create a scene
	auto scene = std::make_unique<Scene>("DebugTimeScene");

	// Create a moving object to visualize time scaling
	auto movingObject = std::make_unique<GameObject>();
	movingObject->setName("MovingObject");
	movingObject->getTransform()->setPosition({100.0f, SCREEN_HEIGHT / 2.0f});
	movingObject->getTransform()->setScale({1.0f, 1.0f});
	movingObject->addComponent<ShapeRenderer>()->setCircle(30.0f).setColor(
		Color::red());
	movingObject->addComponent<MovingObjectBehaviour>(200.0f,
													  SCREEN_WIDTH - 100.0f);

	// Create a debug controller that will move itself to the persistent scene
	// This ensures debug controls work across all scene transitions
	// Note: The name is optional - DebugTimeControlBehaviour will run when
	// paused because it overrides shouldRunWhenPaused() to return true
	auto debugController = std::make_unique<GameObject>();
	debugController->setName(
		"DebugController");	 // Name is optional, just for identification

	// Example: Only pause + slow/fast (disable normal, very slow, very fast)
	debugController->addComponent<DebugTimeControlBehaviour>(
		KeyCode::SPACE,					 // Pause key
		std::nullopt,					 // Normal speed (disabled)
		KeyCode::NUMBER_2_AND_AT,		 // Slow (enabled)
		KeyCode::NUMBER_3_AND_HASHMARK,	 // Very slow (enabled)
		KeyCode::NUMBER_4_AND_DOLLAR,	 // Fast (enabled)
		std::nullopt,					 // Very fast (disabled)
		true							 // Print menu
	);

	scene->addGameObject(std::move(movingObject));
	// Add debug controller to regular scene - it will move itself to persistent
	// scene on first update (see DebugTimeControlBehaviour implementation)
	scene->addGameObject(std::move(debugController));

	game->addScene(std::move(scene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
