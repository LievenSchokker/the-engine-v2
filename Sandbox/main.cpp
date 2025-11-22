#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"
#include "box2d/box2d.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>


int main()
{
	SdlContext context(SDL_INIT_EVERYTHING);
	SDLRenderer renderer(context);

	WindowOptions options{"Shape Sandbox", 1000, 1000};
	renderer.open(options);
	if ( !renderer.isOpen() ) {
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	SceneManager sceneManager;
	sceneManager.setRenderer(&renderer);
	sceneManager.setClearColor(Color::black());

	auto prototypeScene = std::make_unique<Scene>("PrototypeScene");

	auto circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({150.0, 140.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(50.0).setColor(
		Color::lightBlue());

	prototypeScene->addGameObject(std::move(circle));

	sceneManager.addScene(std::move(prototypeScene));
	sceneManager.setActiveScene("PrototypeScene");

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();
	Color clearColor = Color::darkGray();

	InputManager* input = InputManager::getInstance();
	Scene* activeScene = sceneManager.getActiveScene();


	PhysicsSystem physics = PhysicsSystem();
	Box2DPhysicsWorld* world = physics.GetWorld();
	b2BodyId bodyId = world->addBody(circle->getTransform());


	while ( running && renderer.isOpen() ) {
		input->update();


		world->update();
		b2Vec2 position = b2Body_GetPosition(bodyId);
		b2Rot rotation = b2Body_GetRotation(bodyId);



		GameObject* circle = activeScene->getGameObject("BlueCircle");
		circle->getTransform()->setPosition(Vector2(position.x + 100, position.y));
		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));
		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime =
			static_cast<float>(currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		sceneManager.update(deltaTime);
		sceneManager.render();

		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) ) {
			running = false;
		}

		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}
