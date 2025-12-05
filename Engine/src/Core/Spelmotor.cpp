#include "Game.h"
#include "Core/SpelMotor.h"

#include "../../inc/Audio/Components/MusicSource.h"
#include "Audio/Components/SoundSource.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SDLBackendContext.h"
#include "Input/InputManager.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Networking/Server/Server.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"

#include <iostream>
#include <chrono>

#include "Core/IEngineLoop.h"
#include "Core/EngineLoopFactory.h"
#include "Scene/SceneManager.h"

SpelMotor::SpelMotor(std::unique_ptr<Game> game)
	:
	running(false),
	specifications(game->getApplicationSpecifications()),
	coreSystemLoop(
		EngineLoopFactory::createEngineLoop(std::move(game))),
	coreClock(std::make_unique<ApplicationClock>(coreSystemLoop->getClock(),
	                                             specifications.
	                                             networkingOptions.tickRate,
	                                             specifications.
	                                             maxFrameTime))
{
	if (coreSystemLoop == nullptr)
	{
		throw std::runtime_error(
			"Core System Loop is null double check your applicationSpecifications.");
	}
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

	while (running)
	{
		coreClock->tick();

		while (coreClock->shouldFixedUpdate())
		{
			coreSystemLoop->fixedUpdate(coreClock->getDeltaTime());
			coreClock->consumeFixedUpdate();
		}

		coreSystemLoop->update(coreClock->getDeltaTime());
	}
}

void SpelMotor::shutdown() const
{
	coreSystemLoop->shutdown();
}