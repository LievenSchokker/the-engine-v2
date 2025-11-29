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
      tickRate(applicationSpecifications.tickRate),
      physicsWorld(std::make_unique<Box2DPhysicsWorld>(
          applicationSpecifications.tickRate))
{
    if (applicationSpecifications.renderBackend == RenderBackend::SDL)
    {
        SdlContext context = SdlContext();
        timer.reset();
        timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []() {
            // Get Ticks returns ms we need seconds;
            return (SDL_GetTicks() / 1000.0);
        });

        renderer = std::make_unique<SDLRenderer>(context);
    }
}


SpelMotor::~SpelMotor() = default;

void SpelMotor::run()
{
    timer->start();

    // TODO Server or Client -> Start()
    physicsWorld->start();
    // TODO SceneManager -> Start()

    renderer->open(specifications.windowOptions);

    InputManager::getInstance();
    update();
}


void SpelMotor::update()
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
            physicsWorld->update();
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
    physicsWorld->shutdown();
    // TODO server->shutdown() and client->shutdown()
}