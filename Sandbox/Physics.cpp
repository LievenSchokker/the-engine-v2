#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Math/Vector2.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

// This has been added because sometimes SDL causes main to be redefined.
// Which then causes linking error's
#undef main

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

int main()
{
	SdlContext context(SDL_INIT_EVERYTHING);
	SDLRenderer renderer(context);
	InputManager* input = InputManager::getInstance();

	WindowOptions options{"Shape Sandbox", 500, 500};
	renderer.open(options);
	if ( !renderer.isOpen() )
	{
		if ( !renderer.isOpen() )
		{
			std::cout << "Failed to open SDL window\n";
			return 1;
		}

		SceneManager sceneManager;
		// sceneManager.setRenderer(&renderer);
		sceneManager.setClearColor(Color::black());

		auto scene = std::make_unique<Scene>("PrototypeScene");

		std::unique_ptr<GameObject> circle;
		std::unique_ptr<GameObject> rectangle;

		createCircle(circle);
		createRectangle(rectangle);

		scene->addGameObject(std::move(circle));
		scene->addGameObject(std::move(rectangle));

		sceneManager.addScene(std::move(scene));
		sceneManager.setActiveScene("PrototypeScene");

		Scene* activeScene = sceneManager.getActiveScene();
		GameObject* circleGO = activeScene->getGameObject("BlueCircle");
		GameObject* rectangleGO = activeScene->getGameObject("OrangeSquare");

		// --- Physics World ---
		std::unique_ptr<IPhysicsWorld> physicsWorld =
			std::make_unique<Box2DPhysicsWorld>();
		std::unique_ptr<IPhysicsWorld> physicsWorld =
			std::make_unique<Box2DPhysicsWorld>();
		physicsWorld->start();

		// Register GameObjects directly
		physicsWorld->createBody(circleGO->getComponent<RigidBody>());
		physicsWorld->createBody(rectangleGO->getComponent<RigidBody>());
		physicsWorld->createBody(circleGO->getComponent<RigidBody>());
		physicsWorld->createBody(rectangleGO->getComponent<RigidBody>());

		bool running = true;
		Uint32 lastTicks = SDL_GetTicks();

		while ( running && renderer.isOpen() )
		{
			while ( running && renderer.isOpen() )
			{
				input->update();

				// --- Physics update ---
				physicsWorld->fixedUpdate();

				// Clock
				Uint32 currentTicks = SDL_GetTicks();
				float deltaTime = (currentTicks - lastTicks) / 1000.0f;
				lastTicks = currentTicks;

				// Update and render
				sceneManager.update(deltaTime);

				// Build render queue
				RenderQueue queue;
				sceneManager.buildRenderQueue(queue);

				// --- Rendering ---
				renderer.beginFrame(sceneManager.getClearColor());
				executeRenderQueue(renderer, queue);
				renderer.presentFrame();

				// Input
				if ( input->quitRequested() ||
					 input->wasKeyPressed(KeyCode::ESCAPE) )
				{
					if ( input->quitRequested() ||
						 input->wasKeyPressed(KeyCode::ESCAPE) )
					{
						running = false;
					}

					if ( input->wasKeyPressed(KeyCode::W) )
					{
						if ( input->wasKeyPressed(KeyCode::W) )
						{
							physicsWorld->destroyBody(
								rectangleGO->getComponent<RigidBody>());
						}

						if ( input->wasKeyPressed(KeyCode::SPACE) )
						{
							if ( input->wasKeyPressed(KeyCode::SPACE) )
							{
								constexpr Vector2 force = {0, 100};
								physicsWorld->applyForce(
									rectangleGO->getComponent<RigidBody>(),
									force);
								physicsWorld->applyForce(
									rectangleGO->getComponent<RigidBody>(),
									force);
							}

							SDL_Delay(16);
						}

						renderer.close();
						return 0;
					}