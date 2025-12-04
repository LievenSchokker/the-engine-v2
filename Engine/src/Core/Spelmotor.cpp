#include "Game.h"
#include "Core/SpelMotor.h"

#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"

#include <iostream>
#include <ostream>

#include "Input/SDLInputAdapter.h"

SpelMotor::SpelMotor(std::unique_ptr<Game> gameArgument)
	:
	game{std::move(gameArgument)},
	specifications(game->getApplicationSpecifications()),
	running(false),
	timer(nullptr),
	tickRate(specifications.tickRate),
	physicsWorld(std::make_unique<Box2DPhysicsWorld>(
		specifications.tickRate)),
	sceneManager(std::make_unique<SceneManager>())
{
}


SpelMotor::~SpelMotor() = default;

void SpelMotor::start()
{

	if (specifications.renderBackend == RenderBackend::SDL)
	{
		SdlContext context = SdlContext();
		timer.reset();

		// TODO SDL Injection layer
		clockFunction = []()
		{
			return SDL_GetTicks() / 1000.0;
		};
		timer = std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);

		renderer = std::make_unique<SDLRenderer>(context);

	}
	renderer->open(specifications.windowOptions);
	timer->start();

	// TODO Server or Client -> Start()
	physicsWorld->start();

	initFirstGameScene();
	run();
}

void SpelMotor::run()
{
	running = true;

	InputManager* input = InputManager::getInstance();

	while (running)
	{
		timer->tick();

		if (input->quitRequested())
		{
			running = false;
		}

		while (timer->shouldFixedUpdate())
		{
			input->update();
			physicsWorld->fixedUpdate();
			timer->consumeFixedUpdate();
		}

		// TODO Network->Update()
		// TODO Audio->Update();
		renderer->presentFrame();
	}
}

void SpelMotor::shutdown()
{
	running = false;
	// TODO audioSystem->shutdown()
	InputManager::shutdown();
	renderer->close();
	physicsWorld->shutdown();
	// TODO scenemanager->shutdown()
	// TODO server->shutdown() and client->shutdown()
}

void SpelMotor::initFirstGameScene() const
{
	std::unique_ptr<Scene> scene = game->getFirstScene();

	if (scene == nullptr)
	{
		throw std::runtime_error("Game needs at least one scene to start!");
	}

	std::string sceneName = scene->getName();
	sceneManager->addScene(std::move(scene));
	sceneManager->setActiveScene(sceneName);
}