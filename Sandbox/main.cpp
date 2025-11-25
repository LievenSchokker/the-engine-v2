#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsSystem.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "GameObject/Vector2.h"


void createCircle(std::unique_ptr<GameObject>& circle)
{
	circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50).setColor(
		Color::lightBlue());

	// Physics components
	circle->addComponent<RigidBody>(); // dynamic by default
	circle->addComponent<Collider>()->setCircle(50); // collider radius 50
}

void createRactangle(std::unique_ptr<GameObject>& rectangle)
{
	rectangle = std::make_unique<GameObject>();
	rectangle->setName("OrangeSquare");
	rectangle->getTransform()->setPosition({30.0, 450.0});

	rectangle->addComponent<ShapeRenderer>()->setRectangle({400, 50}).setColor(
		Color::orange());

	// Physics components
	auto rb = rectangle->addComponent<RigidBody>();
	rb->makeStatic();

	rectangle->addComponent<Collider>()->setRectangle({400, 50}); // full size
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SdlContext context(SDL_INIT_EVERYTHING);
	SDLRenderer renderer(context);
	InputManager* input = InputManager::getInstance();

	WindowOptions options{"Shape Sandbox", 500, 500};
	renderer.open(options);
	if (!renderer.isOpen()) {
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	// Create scene manger
	SceneManager sceneManager;
	sceneManager.setRenderer(&renderer);
	sceneManager.setClearColor(Color::black());

	auto prototypeScene = std::make_unique<Scene>("PrototypeScene");

	std::unique_ptr<GameObject> circle;
	createCircle(circle);

	std::unique_ptr<GameObject> rectangle;
	createRactangle(rectangle);

	// Add game object to scene
	prototypeScene->addGameObject(std::move(circle));
	prototypeScene->addGameObject(std::move(rectangle));

	// Add scene to scene manger
	sceneManager.addScene(std::move(prototypeScene));
	sceneManager.setActiveScene("PrototypeScene");

	// -------------------------------------

	Scene* activeScene = sceneManager.getActiveScene();
	GameObject* circleNew = activeScene->getGameObject("BlueCircle");
	GameObject* rectangleNew = activeScene->getGameObject("OrangeSquare");

	// Physics
	std::unique_ptr<PhysicsSystem> physics = std::make_unique<PhysicsSystem>();
	physics->start();

	// Register physics body
	physics->registerBody(circleNew);
	physics->registerBody(rectangleNew);

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();

	while (running && renderer.isOpen()) {
		input->update();

		// Update the physics world -- TODO WILL CRASH IF NO OBJECTS IN THE WORLD
		physics->update(lastTicks);
		physics->syncData();

		// Clock
		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime = static_cast<float>(currentTicks - lastTicks) /
		                  1000.0f;
		lastTicks = currentTicks;

		// Update scenes
		sceneManager.update(deltaTime);

		// Render
		sceneManager.render();

		// Input
		if (input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE)) {
			running = false;
		}
		if (input->wasKeyPressed(KeyCode::W)) {
			physics->unregisterBody(rectangleNew);
		}
		if (input->wasKeyPressed(KeyCode::SPACE)) {
			constexpr Vector2 force = Vector2(0, 100);
			physics->applyForce(rectangleNew, force);
		}

		physics->syncData();


		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}