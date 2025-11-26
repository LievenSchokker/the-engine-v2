#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"

#include <iostream>
#include <ostream>

#include "Input/SDLInputAdapter.h"

SpelMotor::SpelMotor(ApplicationSpecifications const applicationSpecifications)
	: running(false),
	  specifications(applicationSpecifications),
	  timer(nullptr),
	  tickRate(applicationSpecifications.tickRate)
{
	if (applicationSpecifications.renderBackend == RenderBackend::SDL) {
		SdlContext context = SdlContext();
		timer.reset();
		timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []() {
			//Get Ticks retuns ms we need seconds;
	  physicsWorld(std::make_unique<Box2DPhysicsWorld>(applicationSpecifications.tickRate))
			return (SDL_GetTicks() / 1000.0);
		});

		renderer = std::make_unique<SDLRenderer>(context);
	}
}


SpelMotor::~SpelMotor() = default;


void SpelMotor::run()
{
	timer->start();

	//TODO Server or Client -> Start()
	//TODO SceneManager -> Start()
	renderer->open(specifications.windowOptions);
	update();
}

	physicsWorld->start();
			InputManager::getInstance()->update();
			physicsWorld->update();

			timer->consumeFixedUpdate();
	}
}

		while (timer->shouldFixedUpdate()) {
		}
void SpelMotor::shutdown()
{
	running = false;

	//TODO audioSystem->shutdown()
	InputManager::shutdown();
	renderer->close();
	//TODO scenemanager->shutdown()
	//TODO physicsWorld->shutdown()
	//TODO server->shutdown() and client->shutdown()
}


void SpelMotor::update()
{
	running = true;

	while (running) {
		timer->tick();

		//TODO REPLACE THIS WITH EVENTMANAGER
		InputManager::getInstance()->update();
		while (timer->shouldFixedUpdate()) {
			//TODO Physics->Update();

			timer->consumeFixedUpdate();
		}

		//TODO Network->Update()
		//TODO Audio->Update();
		renderer->presentFrame();

		if (InputManager::getInstance()->quitRequested()) {
			shutdown();
		}
	}
	physicsWorld->shutdown();
}