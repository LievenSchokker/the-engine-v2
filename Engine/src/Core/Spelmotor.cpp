#include "nuklear.h"
#include "nuklear_sdl_renderer.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"

#include <iostream>
#include <ostream>

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
        timer = std::make_unique<ApplicationClock>(1.0f / tickRate, []()
        {
            //Get Ticks retuns ms we need seconds;
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
    //TODO Physics -> Start()
    //TODO SceneManager -> Start()
    renderer->open(specifications.windowOptions);
    InputManager::getInstance();
    update();
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

    while (running)
    {
        timer->tick();

        //TODO REPLACE THIS WITH EVENTMANAGER
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                shutdown();
            }
            nk_sdl_handle_event(&event);
        }

        while (timer->shouldFixedUpdate())
        {
            InputManager::getInstance()->update();
            //TODO Physics->Update();

            timer->consumeFixedUpdate();
        }


        //TODO Network->Update()
        //TODO Audio->Update();
        renderer->presentFrame();
    }
}
