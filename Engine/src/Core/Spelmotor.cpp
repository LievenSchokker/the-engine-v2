#include "Core/SpelMotor.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SdlContext.h"
#include "Input/InputManager.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Networking/Server/Server.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

#include "Core/EngineLoop.h"
#include "Core/EngineLoopFactory.h"
#include "Scene/SceneManager.h"

SpelMotor::SpelMotor(ApplicationSpecifications applicationSpecifications)
	: specifications(applicationSpecifications),
    running(false),
    coreSystemLoop(EngineLoopFactory::createEngineLoop(applicationSpecifications.networkingOptions.mode)),
    coreClock(std::make_unique<ApplicationClock>(coreSystemLoop->getClock(),
        applicationSpecifications.networkingOptions.tickRate,
        applicationSpecifications.maxFrameTime))
{
    if (coreSystemLoop == nullptr)
    {
        throw std::runtime_error("Core System Loop is null double check your applicationSpecifications.");
    }
}

SpelMotor::~SpelMotor()
{
	shutdown();
}

void SpelMotor::start()
{
    coreClock->start();
    run();
}

void SpelMotor::run()
{
    running = true;

    while (running)
    {
        coreClock->tick();

        while (coreClock->shouldFixedUpdate())
        {
            coreSystemLoop->fixedUpdate();
            coreClock->consumeFixedUpdate();
        }
        coreSystemLoop->update();
    }
}

void SpelMotor::shutdown() const
{
    coreSystemLoop->shutdown();
}
