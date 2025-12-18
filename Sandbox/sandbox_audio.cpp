#include "Audio/Components/MusicSource.h"
#include "Audio/Components/SoundSource.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "External/SDLBackendContext.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Rendering/Color.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/Window/WindowOptions.h"
#include "Scene/SceneManager.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#undef main

// -------------------- Create Simple Visuals --------------------
std::unique_ptr<GameObject> createCircle()
{
	auto circle = std::make_unique<GameObject>();
	circle->setName("BlueCircle");
	circle->getTransform()->setPosition({200.0f, 200.0f});

	auto renderer = circle->addComponent<ShapeRenderer>();
	renderer->setCircle(60).setColor(Color::lightBlue());

	return circle;
}

std::unique_ptr<GameObject> createRectangle()
{
	auto rect = std::make_unique<GameObject>();
	rect->setName("OrangeBox");
	rect->getTransform()->setPosition({100.0f, 350.0f});

	auto renderer = rect->addComponent<ShapeRenderer>();
	renderer->setRectangle({300, 80}).setColor(Color::orange());

	return rect;
}

// --------------------------- MAIN -----------------------------
int main()
{
	// --- SDL INIT ---
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0 )
	{
		std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
		return 1;
	}

	InputManager* input = InputManager::getInstance();

	// --- RENDERING ---
	SDLBackendContext backendContext = SDLBackendContext();
	SDLRenderer renderer = SDLRenderer(backendContext);
	WindowOptions windowOpts{"Audio + Scene Example", 800, 600};

	renderer.open(windowOpts);
	if ( !renderer.isOpen() )
	{
		std::cout << "Failed to open SDL window.\n";
		return 1;
	}

	// --- AUDIO ---
	auto backend = std::make_unique<AudioBackendSDL>();
	auto audioManager = std::make_unique<AudioManager>();
	audioManager->initialize(std::move(backend));

	// auto music = std::make_unique<MusicSource>(audioManager.get());
	MusicSource* music = new MusicSource(audioManager.get());
	music->setLoop(true);
	music->loadMusic("Assets/music_jam.wav");
	music->play();

	auto effect = std::make_unique<SoundSource>(audioManager.get());
	effect->loadSound("Assets/audio_effect_pigeons_flying.wav");

	// --- MAIN LOOP ---
	bool running = true;

	while ( running )
	{
		input->update();

		if ( input->quitRequested() || input->wasKeyPressed(KeyCode::ESCAPE) )
		{
			running = false;
		}

		// Audio controls
		if ( input->wasKeyPressed(KeyCode::SPACE) ) music->pause();
		if ( input->wasKeyPressed(KeyCode::C) ) music->resume();
		if ( input->wasKeyPressed(KeyCode::P) ) effect->play();
		if ( input->wasKeyPressed(KeyCode::ESCAPE) ) delete music;
		if ( input->wasKeyPressed(KeyCode::UP_ARROW) )
			music->setVolume(music->getCurrentVolume() + 0.1f);
		if ( input->wasKeyPressed(KeyCode::DOWN_ARROW) )
			music->setVolume(music->getCurrentVolume() - 0.1f);
	}

	// --- SHUTDOWN ---
	audioManager->shutdown();
	renderer.close();
	SDL_Quit();

	return 0;
}
