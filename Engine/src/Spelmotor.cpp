#include "ApplicationSpecifications.h"
#include "SpelMotor.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Rendering/SDL/SDLRender.h"
#include "Rendering/IRender.h"

SpelMotor::SpelMotor(ApplicationSpecifications const applicationSpecifications)
    : running(false),
      specifications(applicationSpecifications),
      frameCounter(0),
      frameTime(0),
      lastFrameTime(0),
      timeStep(0)
{
    if (applicationSpecifications.renderBackend == RenderBackend::SDL)
    {
        SdlContext context = SdlContext();
        renderer = std::make_unique<SDLRender>(SDLRender(context));
    }
}

void SpelMotor::run()
{
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
    lastFrameTime = SDL_GetTicks();

    while (running)
    {
        //Update Systems
        InputManager::getInstance()->update();
        //TODO Network->Update()
        //TODO Physics->Update();
        //TODO Audio->Update();
        renderer->presentFrame();



        //TODO abstract Timer class
        float time = SDL_GetTicks();
        frameTime = time - lastFrameTime;
        timeStep = std::min<float>(lastFrameTime, 0.0333f);
        lastFrameTime = time;
        frameCounter++;


        //TODO REPLACE THIS WITH EVENTMANAGER
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                shutdown();
            }
        }
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
