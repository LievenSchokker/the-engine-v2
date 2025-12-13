#include "Core/SpelMotor.h"

#include "Audio/Components/MusicSource.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/EngineLoopFactory.h"
#include "Core/IEngineLoop.h"
#include "Game.h"

#include <chrono>

SpelMotor::SpelMotor(std::unique_ptr<Game> game)
	: running(false),
	  specifications(game->getApplicationSpecifications()),
	  coreSystemLoop(EngineLoopFactory::createEngineLoop(std::move(game))),
	  coreClock(std::make_unique<ApplicationClock>(
		  coreSystemLoop->getClock(), specifications.networkingOptions.tickRate,
		  specifications.maxFrameTime))
{
	if ( coreSystemLoop == nullptr )
	{
		throw std::runtime_error(
			"Core System Loop is null double check your "
			"applicationSpecifications.");
	}

	// Set the clock reference in GameWorld for debug access
	coreSystemLoop->setApplicationClock(coreClock.get());
}

SpelMotor::~SpelMotor()
{
	shutdown();
}

void SpelMotor::start()
{
	coreClock->start();
	coreSystemLoop->start();  // what does this do?
	run();
}

void SpelMotor::run()
{
	running = true;

	while ( running )
	{
		coreClock->tick();

		// Poll input BEFORE fixedUpdate so behaviors can read current frame's
		// input This ensures input is available when behaviors run in
		// fixedUpdate
		coreSystemLoop->update(coreClock->getDeltaTime());

		while ( coreClock->shouldFixedUpdate() )
		{
			coreSystemLoop->fixedUpdate(coreClock->getDeltaTime());
			coreClock->consumeFixedUpdate();
		}
	}
}

void SpelMotor::shutdown() const
{
	running = false;
	coreSystemLoop->shutdown();
}

ApplicationClock* SpelMotor::getClock()
{
	return coreClock.get();
}