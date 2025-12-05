#include "Core/SpelMotor.h"

#include "../../inc/Audio/Components/MusicSource.h"
#include "Audio/Components/SoundSource.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/ApplicationClock.h"
#include "Core/ApplicationSpecifications.h"
#include "External/SDLBackendContext.h"
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
	if ( applicationSpecifications.renderBackend == RenderBackend::SDL )
	{
		backendContext = std::make_unique<SDLBackendContext>();
		timer.reset();

		// TODO SDL Injection layer
		clockFunction = []() { return SDL_GetTicks() / 1000.0; };
		timer = std::make_unique<ApplicationClock>(clockFunction, 60, 0.25);

		renderer = std::make_unique<SDLRenderer>(*backendContext);

		// Audio
		auto audioBackend = std::make_unique<AudioBackendSDL>();
		audioManager = std::make_unique<AudioManager>();
		audioManager->initialize(std::move(audioBackend));
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

	while ( running )
	{
		timer->tick();

		if ( input->quitRequested() )
		{
			running = false;
		}

		while ( timer->shouldFixedUpdate() )
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
	physicsWorld->shutdown();
	// TODO scenemanager->shutdown()
	// TODO server->shutdown() and client->shutdown()
}