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

	// --- Create background music source ---
	auto music = std::make_unique<MusicSource>(audioManager.get());
	music->setLoop(true);
	music->loadMusic(
		R"(C:\Users\thijs\content\minor\project\the-engineV2\Sandbox\assets\music_jam.wav)");
	music->play();

	// Create explosion sound
	auto discover = std::make_unique<SoundSource>(audioManager.get());
	discover->loadSound(
		R"(C:\Users\thijs\content\minor\project\the-engineV2\Sandbox\assets\audio_effect_voice.wav)");

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

		if ( input->isKeyDown(KeyCode::SPACE) )
		{
			music->pause();
		}
		if ( input->isKeyDown(KeyCode::C) )
		{
			music->resume();
		}

		if ( input->isKeyDown(KeyCode::P) )
		{
			discover->play();
		}
		if ( input->isKeyDown(KeyCode::UP_ARROW) )
		{
			float volume = music->getCurrentVolume();
			volume += 0.1f;
			music->setVolume(volume);
		}
		if ( input->isKeyDown(KeyCode::DOWN_ARROW) )
		{
			discover->stop();
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