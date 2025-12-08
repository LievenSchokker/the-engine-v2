#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SdlContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <memory>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 480

int main()
{
	SdlContext context(SDL_INIT_VIDEO);
	SDLRenderer renderer(context);

	WindowOptions options{"Shape Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT};
	renderer.open(options);
	if ( !renderer.isOpen() )
	{
		std::cout << "Failed to open SDL window\n";
		return 1;
	}

	SceneManager sceneManager;
	sceneManager.setClearColor(Color::black());

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

	prototypeScene->addGameObject(std::move(circle));
	prototypeScene->addGameObject(std::move(rectangle));

	sceneManager.addScene(std::move(prototypeScene));
	sceneManager.setActiveScene("PrototypeScene");

	bool running = true;
	Uint32 lastTicks = SDL_GetTicks();
	Color clearColor = Color::darkGray();
	RenderQueue renderQueue;

	InputManager* input = InputManager::getInstance();
	Scene* activeScene = sceneManager.getActiveScene();

	while ( running && renderer.isOpen() )
	{
		input->update();

		// ================================ GAME CODE CHECK
		// ================================

		if ( input->wasKeyPressed(KeyCode::SPACE) )
		{
			clearColor = (clearColor == Color::darkGreen())
							 ? Color::darkPurple()
							 : Color::darkGreen();
			sceneManager.setClearColor(clearColor);
		}

		if ( input->wheelDeltaY() != 0 )
		{
			GameObject* rectangle =
				activeScene->getGameObject("YellowRectangle");
			if ( rectangle != nullptr )
			{
				double currentRotation =
					rectangle->getTransform()->getRotationAngle();
				rectangle->getTransform()->setRotationAngle(
					currentRotation + input->wheelDeltaY());
			}
		}

		if ( input->wheelDeltaX() != 0 )
		{
			GameObject* circle = activeScene->getGameObject("BlueCircle");
			if ( circle != nullptr )
			{
				Vector2 currentPosition = circle->getTransform()->getPosition();
				circle->getTransform()->setPosition(
					{currentPosition.x() + input->wheelDeltaX() * 10,
					 currentPosition.y()});
				Vector2 currentScale = circle->getTransform()->getScale();
				circle->getTransform()->setScale(
					{currentScale.x() + input->wheelDeltaX() * 0.1f,
					 currentScale.y() + input->wheelDeltaX() * 0.1f});
					;
			}
		}

		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime =
			static_cast<float>(currentTicks - lastTicks) / 1000.0f;
		lastTicks = currentTicks;

		sceneManager.update(deltaTime);
		sceneManager.buildRenderQueue(renderQueue);
		executeRenderQueue(renderer, renderQueue);

		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) )
		{
			running = false;
		}

		SDL_Delay(16);
	}

	renderer.close();
	return 0;
}
