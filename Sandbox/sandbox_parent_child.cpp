#include "Behaviour/Behaviour.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "EntryPoint.h"
#include "Game.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

/**
 * @brief Behavior class that handles input for the parent-child demo.
 *
 * Handles:
 * - WASD: Move parent GameObject
 * - Arrow Keys: Move child1 GameObject (in local space)
 * - Q/E: Rotate parent GameObject
 * - Z/X: Scale parent GameObject
 * - SPACE: Toggle clear color
 * - R: Reset parent transform
 * - 1/2/3: Attach/detach children
 */
class ParentChildInputBehaviour: public Behaviour
{
   public:
	explicit ParentChildInputBehaviour(Scene* scene)
		: scene(scene),
		  inputManager(nullptr),
		  clearColor(Color::darkGray()),
		  parentObj(nullptr),
		  child1(nullptr),
		  child2(nullptr),
		  child3(nullptr)
	{
	}

	~ParentChildInputBehaviour() override = default;

	void onAwake() override
	{
		inputManager = InputManager::getInstance();

		if ( scene != nullptr )
		{
			parentObj = scene->getGameObject("ParentObject");
			child1 = scene->getGameObject("Child1");
			child2 = scene->getGameObject("Child2");
			child3 = scene->getGameObject("Child3");
		}

		std::cout << "Parent-Child Hierarchy Demo\n";
		std::cout << "==========================\n";
		std::cout << "Controls:\n";
		std::cout << "  WASD - Move parent\n";
		std::cout << "  Arrow Keys - Move child1 (in local space)\n";
		std::cout << "  Q/E - Rotate parent (left/right)\n";
		std::cout << "  Z/X - Scale parent (smaller/larger)\n";
		std::cout << "  R - Reset all transforms\n";
		std::cout << "  1/2/3 - Toggle child attachment\n";
		std::cout << "  SPACE - Toggle clear color\n";
		std::cout << "  ESC - Exit\n\n";
		std::cout << "Test Scenarios:\n";
		std::cout << "  1. Rotate parent (Q/E) - child1 should orbit\n";
		std::cout << "  2. Move child1 (arrows) then rotate parent - orbit "
					 "should maintain offset\n";
		std::cout << "  3. Detach child1 (1) while moving - should preserve "
					 "world position\n\n";
	}

	void update(float deltaTime, GameWorld* world) override
	{
		(void)deltaTime;

		if ( inputManager == nullptr || scene == nullptr ||
			 parentObj == nullptr )
		{
			return;
		}

		Transform* parentTransform = parentObj->getTransform();
		if ( parentTransform == nullptr )
		{
			return;
		}

		// SPACE key reserved for future use
		(void)clearColor;

		// Handle WASD keys to move parent
		const float moveSpeed = 100.0f * deltaTime;
		Vector2 currentPos = parentTransform->getPosition();
		Vector2 newPos = currentPos;

		if ( inputManager->isKeyDown(KeyCode::A) )
		{
			newPos.x -= moveSpeed;
		}
		if ( inputManager->isKeyDown(KeyCode::D) )
		{
			newPos.x += moveSpeed;
		}
		if ( inputManager->isKeyDown(KeyCode::W) )
		{
			newPos.y -= moveSpeed;
		}
		if ( inputManager->isKeyDown(KeyCode::S) )
		{
			newPos.y += moveSpeed;
		}

		if ( newPos.x != currentPos.x || newPos.y != currentPos.y )
		{
			parentTransform->setPosition(newPos);
		}

		// Handle Arrow keys to move child1 (in local space)
		if ( child1 != nullptr )
		{
			Transform* child1Transform = child1->getTransform();
			if ( child1Transform != nullptr )
			{
				const float childMoveSpeed = 100.0f * deltaTime;
				Vector2 childCurrentPos = child1Transform->getPosition();
				Vector2 childNewPos = childCurrentPos;

				if ( inputManager->isKeyDown(KeyCode::LEFT_ARROW) )
				{
					childNewPos.x -= childMoveSpeed;
				}
				if ( inputManager->isKeyDown(KeyCode::RIGHT_ARROW) )
				{
					childNewPos.x += childMoveSpeed;
				}
				if ( inputManager->isKeyDown(KeyCode::UP_ARROW) )
				{
					childNewPos.y -= childMoveSpeed;
				}
				if ( inputManager->isKeyDown(KeyCode::DOWN_ARROW) )
				{
					childNewPos.y += childMoveSpeed;
				}

				if ( childNewPos.x != childCurrentPos.x ||
					 childNewPos.y != childCurrentPos.y )
				{
					// Setting local position - transform system will handle
					// world space conversion
					child1Transform->setPosition(childNewPos);
				}
			}
		}

		// Handle Q/E to rotate parent
		const double rotationSpeed = 90.0 * deltaTime;	// degrees per second
		double currentRotation = parentTransform->getRotationAngle();

		if ( inputManager->isKeyDown(KeyCode::Q) )
		{
			parentTransform->setRotationAngle(currentRotation - rotationSpeed);
		}
		if ( inputManager->isKeyDown(KeyCode::E) )
		{
			parentTransform->setRotationAngle(currentRotation + rotationSpeed);
		}

		// Handle Z/X to scale parent
		const float scaleSpeed = 0.5f * deltaTime;
		Vector2 currentScale = parentTransform->getScale();

		if ( inputManager->isKeyDown(KeyCode::Z) )
		{
			float newScaleX = std::max(0.1f, currentScale.x - scaleSpeed);
			float newScaleY = std::max(0.1f, currentScale.y - scaleSpeed);
			parentTransform->setScale({newScaleX, newScaleY});
		}
		if ( inputManager->isKeyDown(KeyCode::X) )
		{
			float newScaleX = std::min(3.0f, currentScale.x + scaleSpeed);
			float newScaleY = std::min(3.0f, currentScale.y + scaleSpeed);
			parentTransform->setScale({newScaleX, newScaleY});
		}

		// Handle R to reset all transforms
		if ( inputManager->wasKeyPressed(KeyCode::R) )
		{
			// Reset parent transform
			parentTransform->setPosition(
				{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
			parentTransform->setRotationAngle(0.0);
			parentTransform->setScale({1.0f, 1.0f});

			// Reset child1 transform
			if ( child1 != nullptr )
			{
				Transform* child1Transform = child1->getTransform();
				if ( child1Transform != nullptr )
				{
					child1Transform->setPosition({80.0f, 0.0f});
					child1Transform->setRotationAngle(0.0);
					child1Transform->setScale({1.0f, 1.0f});
				}
			}

			// Reset child2 transform
			if ( child2 != nullptr )
			{
				Transform* child2Transform = child2->getTransform();
				if ( child2Transform != nullptr )
				{
					child2Transform->setPosition({-80.0f, 0.0f});
					child2Transform->setRotationAngle(45.0);
					child2Transform->setScale({1.0f, 1.0f});
				}
			}

			// Reset child3 transform
			if ( child3 != nullptr )
			{
				Transform* child3Transform = child3->getTransform();
				if ( child3Transform != nullptr )
				{
					child3Transform->setPosition({0.0f, -80.0f});
					child3Transform->setRotationAngle(0.0);
					child3Transform->setScale({0.8f, 0.8f});
				}
			}

			std::cout << "All transforms reset\n";
		}

		// Handle 1/2/3 to toggle child attachment
		if ( inputManager->wasKeyPressed(KeyCode::NUMBER_1_AND_EXCLAMATION) &&
			 child1 != nullptr )
		{
			if ( child1->getParent() == parentObj )
			{
				child1->setParent(nullptr);
				std::cout << "Child1 detached from parent\n";
			}
			else
			{
				child1->setParent(parentObj);
				std::cout << "Child1 attached to parent\n";
			}
		}

		if ( inputManager->wasKeyPressed(KeyCode::NUMBER_2_AND_AT) &&
			 child2 != nullptr )
		{
			if ( child2->getParent() == parentObj )
			{
				child2->setParent(nullptr);
				std::cout << "Child2 detached from parent\n";
			}
			else
			{
				child2->setParent(parentObj);
				std::cout << "Child2 attached to parent\n";
			}
		}

		if ( inputManager->wasKeyPressed(KeyCode::NUMBER_3_AND_HASHMARK) &&
			 child3 != nullptr )
		{
			if ( child3->getParent() == parentObj )
			{
				child3->setParent(nullptr);
				std::cout << "Child3 detached from parent\n";
			}
			else
			{
				child3->setParent(parentObj);
				std::cout << "Child3 attached to parent\n";
			}
		}
	}

   private:
	Scene* scene;
	InputManager* inputManager;
	Color clearColor;

	GameObject* parentObj;
	GameObject* child1;
	GameObject* child2;
	GameObject* child3;
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
	spec.windowOptions = {"Parent-Child Hierarchy Demo", SCREEN_WIDTH,
						  SCREEN_HEIGHT};
	spec.maxFrameTime = 0.1;  // 100ms max frame time

	std::unique_ptr<Game> game = std::make_unique<Game>();

	auto parentChildScene = std::make_unique<Scene>("ParentChildScene");

	// Create parent GameObject (large rectangle in center)
	auto parent = std::make_unique<GameObject>();
	parent->setName("ParentObject");
	parent->getTransform()->setPosition(
		{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
	parent->getTransform()->setRotationAngle(0.0);
	parent->getTransform()->setScale({1.0f, 1.0f});
	auto* parentRenderer = parent->addComponent<ShapeRenderer>();
	parentRenderer->setRectangle({120.0f, 120.0f}).setColor(Color::yellow());

	// Create child 1 (small circle, offset to the right)
	auto child1 = std::make_unique<GameObject>();
	child1->setName("Child1");
	child1->getTransform()->setPosition(
		{80.0f, 0.0f});	 // Local position relative to parent
	child1->getTransform()->setRotationAngle(0.0);
	child1->getTransform()->setScale({1.0f, 1.0f});
	auto* child1Renderer = child1->addComponent<ShapeRenderer>();
	child1Renderer->setCircle(25.0f).setColor(Color::lightBlue());

	// Create child 2 (small rectangle, offset to the left)
	auto child2 = std::make_unique<GameObject>();
	child2->setName("Child2");
	child2->getTransform()->setPosition(
		{-80.0f, 0.0f});  // Local position relative to parent
	child2->getTransform()->setRotationAngle(
		45.0);	// Rotated 45 degrees locally
	child2->getTransform()->setScale({1.0f, 1.0f});
	auto* child2Renderer = child2->addComponent<ShapeRenderer>();
	child2Renderer->setRectangle({40.0f, 40.0f}).setColor(Color::purple());

	// Create child 3 (small circle, offset above)
	auto child3 = std::make_unique<GameObject>();
	child3->setName("Child3");
	child3->getTransform()->setPosition(
		{0.0f, -80.0f});  // Local position relative to parent
	child3->getTransform()->setRotationAngle(0.0);
	child3->getTransform()->setScale({0.8f, 0.8f});	 // Slightly smaller scale
	auto* child3Renderer = child3->addComponent<ShapeRenderer>();
	child3Renderer->setCircle(20.0f).setColor(Color::green());

	// Set up parent-child relationships
	child1->setParent(parent.get());
	child2->setParent(parent.get());
	child3->setParent(parent.get());

	// Create a GameObject for input handling
	auto inputHandler = std::make_unique<GameObject>();
	inputHandler->setName("InputHandler");
	inputHandler->addComponent<ParentChildInputBehaviour>(
		parentChildScene.get());

	// Add GameObjects to scene
	parentChildScene->addGameObject(std::move(parent));
	parentChildScene->addGameObject(std::move(child1));
	parentChildScene->addGameObject(std::move(child2));
	parentChildScene->addGameObject(std::move(child3));
	parentChildScene->addGameObject(std::move(inputHandler));

	game->addScene(std::move(parentChildScene));
	game->setApplicationSpecifications(spec);

	return SpelMotorEntry::main(std::move(game));
}
