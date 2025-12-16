// Core/EngineLoopFactory.cpp
#include "Core/EngineLoopFactory.h"

#include "Audio/AudioSystem.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/ApplicationSpecifications.h"
#include "Core/EngineLoop.h"
#include "Game.h"
#include "External/SDLBackendContext.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/Box2D/Box2DPhysicsWorld.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/SceneManager.h"

std::unique_ptr<IEngineLoop> EngineLoopFactory::createEngineLoop(
	std::unique_ptr<Game> game)
{
	const auto& specs = game->getApplicationSpecifications();

	switch (specs.networkingOptions.mode)
	{
		case EngineMode::CLIENT:
			return createClientLoop(std::move(game));

		case EngineMode::SERVER:
			return createServerLoop(std::move(game));

		default:
			return createClientLoop(std::move(game));
	}
}

std::unique_ptr<IEngineLoop> EngineLoopFactory::createClientLoop(
	std::unique_ptr<Game> game)
{
	auto loop = std::make_unique<EngineLoop>(std::move(game));

	Game* gamePtr = loop->getGame();
	const ApplicationSpecifications& specs = gamePtr->getApplicationSpecifications();

	// 1. Backend context FIRST
	if (specs.renderBackend == RenderBackend::SDL)
	{
		auto backendContext = std::make_unique<SDLBackendContext>();
		auto* contextPtr = backendContext.get();

		loop->setClockFunction([]() { return SDL_GetTicks() / 1000.0; });
		loop->setBackendContext(std::move(backendContext));

		auto sdlRenderer = std::make_unique<SDLRenderer>(*contextPtr);
		sdlRenderer->open(specs.windowOptions);
		loop->addSystem(std::make_unique<RenderSystem>(std::move(sdlRenderer)));

		loop->addSystem(std::make_unique<AudioSystem>(
			std::make_unique<AudioBackendSDL>()));
	}

	// 2. Physics BEFORE SceneManager (so it's initialized when scenes start)
	loop->addSystem(std::make_unique<PhysicsSystem>());

	// 3. SceneManager AFTER Physics
	auto sceneManager = std::make_unique<SceneManager>();
	std::unique_ptr<Scene> scenePtr = gamePtr->getFirstScene();
	std::string sceneName = scenePtr->getName();
	sceneManager->addScene(std::move(scenePtr));
	sceneManager->setActiveScene(sceneName);
	loop->addSystem(std::move(sceneManager));

	loop->addSystem(std::make_unique<Client>(std::make_unique<TransportGNS>()));

	return loop;
}

std::unique_ptr<IEngineLoop> EngineLoopFactory::createServerLoop(
	std::unique_ptr<Game> game)
{
	const auto& specs = game->getApplicationSpecifications();

	auto loop = std::make_unique<EngineLoop>(std::move(game));
	return loop;
}