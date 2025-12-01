#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"


SpelMotor::SpelMotor(ApplicationSpecifications const applicationSpecifications)
    : running(false),
      specifications(applicationSpecifications),
      timer(nullptr),
      tickRate(applicationSpecifications.tickRate)
{
    if (applicationSpecifications.renderBackend == RenderBackend::SDL)
    {
        SdlContext context = SdlContext();
        timer.reset();
        timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []() {
            // Get Ticks returns ms we need seconds;
            return (SDL_GetTicks() / 1000.0);
        });

		//TODO SDL Injection layer
    	clockFunction = []() { return SDL_GetTicks() / 1000.0; };
    	timer = std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);


        renderer = std::make_unique<SDLRenderer>(context);
    }
}


SpelMotor::~SpelMotor() = default;

void SpelMotor::start()
{
    timer->start();

    // TODO Server or Client -> Start()
	physicsWorld->start();
    // TODO SceneManager -> Start()

    renderer->open(specifications.windowOptions);

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
    // TODO scenemanager->shutdown()
    // TODO server->shutdown() and client->shutdown()
}