#include <iostream>
#include <ostream>

#include "Core/ApplicationSpecifications.h"
#include "Core/SpelMotor.h"
#include "Core/Timer.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Rendering/SDL/SDLRender.h"
#include "Rendering/IRender.h"

SpelMotor::SpelMotor(ApplicationSpecifications const applicationSpecifications)
    : running(false),
      specifications(applicationSpecifications),
      timer(nullptr)
{
    if (applicationSpecifications.renderBackend == RenderBackend::SDL)
    {
        SdlContext context = SdlContext();
        timer.reset();
        timer = std::make_unique<Timer>(1.0f / 60.0f, []()
        {
            return SDL_GetTicks();
        });
        renderer = std::make_unique<SDLRender>(SDLRender(context));
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


void SpelMotor::shutdown()
{
    running = false;

    //TODO audioSystem->shutdown()
    InputManager::getInstance()->shutdown();
    renderer->close();
    //TODO scenemanager->shutdown()
    //TODO physicsWorld->shutdown()
    //TODO server->shutdown() and client->shutdown()
}
