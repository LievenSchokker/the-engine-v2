#include "Core/EngineLoopFactory.h"
#include "Audio/AudioSystem.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/EngineLoop.h"
#include "Game.h"
#include "Events/SDL/SDLEventProccesor.h"
#include "External/SDLBackendContext.h"
#include "Input/InputManager.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"
#include "Physics/PhysicsSystem.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/SceneManager.h"

#include <iostream>
#include <ostream>

std::unique_ptr<IEngineLoop> EngineLoopFactory::createEngineLoop(
    std::unique_ptr<Game> game)
{
    auto assets = game->getAssetManager();

    const auto& specs = game->getApplicationSpecifications();
    auto loop = std::make_unique<EngineLoop>(std::move(game));

    GameWorld* gameWorld = loop->getGameWorld();
    gameWorld->setAssetManager(std::move(assets));
    Game* gamePtr = loop->getGame();
    std::unique_ptr<IBackendContext> backendContext{};
    IBackendContext* contextPtr = nullptr;

	// BACKEND CONTEXT
    if (specs.renderSettings.renderBackend == RenderBackend::SDL)
    {
        backendContext = std::make_unique<SDLBackendContext>();
        contextPtr = backendContext.get();
        loop->setClockFunction([]()
        {
            return SDL_GetTicks() / 1000.0;
        });
        loop->setBackendContext(std::move(backendContext));
    }

	// EVENTS
    if (hasFlag(specs.engineSystem, EngineSystem::Events))
    {
        gameWorld->setDispatcher(std::make_unique<EventDispatcher>());

        if (contextPtr != nullptr)
        {
            auto events = std::make_unique<SDLEventProcessor>();
            loop->addSystem(std::move(events));
        }

    	// INPUT (needs the event dispatcher)
    	if (hasFlag(specs.engineSystem, EngineSystem::Input))
    	{
    		auto input = std::make_unique<InputManager>();
    		loop->addSystem(std::move(input));
    	}

    }

	// RENDERING
    if (hasFlag(specs.engineSystem, EngineSystem::Renderer))
    {
        if (contextPtr != nullptr)
        {
            auto sdlRenderer = std::make_unique<SDLRenderer>(*contextPtr);
            sdlRenderer->open(specs.renderSettings.windowOptions);

            auto system = std::make_unique<RenderSystem>(std::move(sdlRenderer));
            system->setClearColor(specs.clearColor);
            gameWorld->render = system.get();

        	if (hasFlag(specs.engineSystem, EngineSystem::Events))
        	{
        		system->setupEvents(*gameWorld->getDispatcher());
        	}
            loop->addSystem(std::move(system));
        }
    }

	// AUDIO
    if (hasFlag(specs.engineSystem, EngineSystem::Audio))
    {
        if (contextPtr != nullptr)
        {
            auto audioSystem = std::make_unique<AudioSystem>(std::make_unique<AudioBackendSDL>());
            gameWorld->audio = audioSystem->getAudioManager();
            loop->addSystem(std::move(audioSystem));
        }
    }

	// PHYSICS
    if (hasFlag(specs.engineSystem, EngineSystem::Physics))
    {
        auto physicsSystem = std::make_unique<PhysicsSystem>();
        loop->addSystem(std::move(physicsSystem));
    }

    auto sceneManager = std::make_unique<SceneManager>(*gameWorld);
    std::unique_ptr<Scene> scenePtr = gamePtr->getFirstScene();

    // SCENE
    if (!scenePtr)
    {
        throw std::runtime_error("Game must have at least one scene");
    }

    const std::string sceneName = scenePtr->getName();
	loop->setPendingSceneName(sceneName);
    sceneManager->addScene(std::move(scenePtr));

    for (auto& scene : gamePtr->getAllScenes())
    {
        sceneManager->addScene(std::move(scene));
    }
    loop->addSystem(std::move(sceneManager));

    // NETWORKING
    if (hasFlag(specs.engineSystem, EngineSystem::NetClient))
    {
        auto client = std::make_unique<Client>(std::make_unique<TransportGNS>());
        loop->addSystem(std::move(client));
    }

	if (hasFlag(specs.engineSystem, EngineSystem::NetServer))
	{
		auto server = std::make_unique<Server>(
			ServerConnectionInformation{
				specs.networkingOptions.port,
				specs.networkingOptions.serverIP
			},
			std::make_unique<TransportGNS>());
		loop->addSystem(std::move(server));
	}



    return loop;
}
