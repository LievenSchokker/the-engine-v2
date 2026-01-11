#include <iostream>
#include <stdexcept>

#include "Core/SpelMotor.h"

#include "Audio/Components/MusicSource.h"
#include "Core/ApplicationClock.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

#include "Core/IEngineLoop.h"
#include "Core/EngineLoopFactory.h"
#include "Core/IEngineLoop.h"
#include "Game.h"

#include <stdexcept>

SpelMotor::SpelMotor(std::unique_ptr<Game> game)
    : specifications(game->getApplicationSpecifications()),
      running(false),
      coreSystemLoop(EngineLoopFactory::createEngineLoop(std::move(game))),
      coreClock(std::make_unique<ApplicationClock>(
          coreSystemLoop->getClock(), specifications.networkingOptions.tickRate,
          specifications.maxFrameTime))
{
    if (coreSystemLoop == nullptr)
    {
        throw std::runtime_error(
            "Core System Loop is null double check your "
            "applicationSpecifications.");
    }

    coreSystemLoop->setApplicationClock(coreClock.get());
}

SpelMotor::SpelMotor(std::unique_ptr<Game> game, std::unique_ptr<IEngineLoop> engineLoop)
    : running(false),
      specifications(game->getApplicationSpecifications()),
      coreSystemLoop(std::move(engineLoop)),
      coreClock(nullptr)
{
    if (coreSystemLoop == nullptr)
    {
        throw std::runtime_error(
            "Core System Loop is null double check your "
            "applicationSpecifications.");
    }

    coreClock = std::make_unique<ApplicationClock>(
        coreSystemLoop->getClock(),
        specifications.networkingOptions.tickRate,
        specifications.maxFrameTime);

    coreSystemLoop->setApplicationClock(coreClock.get());
}

SpelMotor::~SpelMotor()
{
    shutdown();
}

void SpelMotor::start()
{
    coreClock->start();
    coreSystemLoop->start();
    run();
}

void SpelMotor::run()
{
    running = true;

	while ( running )
	{
		tick();
	}
	shutdown();
}

void SpelMotor::shutdown()
{
	running = false;
	coreSystemLoop->shutdown();
}

ApplicationClock* SpelMotor::getClock()
{
	return coreClock.get();
}

void SpelMotor::initialize()
{
	coreClock->start();
	coreSystemLoop->start();
	running = true;
}

void SpelMotor::tick()
{
	if (!running) return;

	coreClock->tick();
	while (coreClock->shouldFixedUpdate())
	{
		coreSystemLoop->fixedUpdate(coreClock->getFixedDeltaTime());
		coreClock->consumeFixedUpdate();

		if (coreSystemLoop->isShutdownRequested())
		{
			running = false;
			break;
		}
	}
	coreSystemLoop->update(coreClock->getDeltaTime());
}