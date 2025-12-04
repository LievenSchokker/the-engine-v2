#include "Core/SpelMotor.h"

#include "Audio/MusicSource.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Audio/SoundSource.h"
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
		R"(C:\Users\thijs\content\minor\project\the-engineV2\Sandbox\assets\music_trailer.ogg)");
	music->play();

	// Create explosion sound
	auto explosion = std::make_unique<SoundSource>(audioManager.get());
	explosion->loadSound(
		R"(C:\Users\thijs\content\minor\project\the-engineV2\Sandbox\assets\audio_effect_voice.wav)");

	// Create pigeons sound
	auto pigeons = std::make_unique<SoundSource>(audioManager.get());
	pigeons->loadSound(
		R"(C:\Users\thijs\content\minor\project\the-engineV2\Sandbox\assets\audio_effect_pigeons_flying.wav)");


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
			explosion->play();
			pigeons->play();
		}

		if ( input->isKeyDown(KeyCode::LEFT_ARROW) )
		{
			explosion->setPanning(1.0f, 0.0f);
		}
		if ( input->isKeyDown(KeyCode::DOWN_ARROW) )
		{
			explosion->setPanning(1.0f, 1.0f);
		}

		if ( input->isKeyDown(KeyCode::RIGHT_ARROW) )
		{
			explosion->setPanning(0.0f, 1.0f);
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