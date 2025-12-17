#include "Core/EngineLoopFactory.h"

#include <iostream>
#include <ostream>

#include "Audio/AudioSystem.h"
#include "Audio/SDL/AudioBackendSDL.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/EngineLoop.h"
#include "Game.h"
#include "External/SDLBackendContext.h"
#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/Server.h"
#include "Physics/PhysicsSystem.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Scene/SceneManager.h"

std::unique_ptr<IEngineLoop> EngineLoopFactory::createEngineLoop(
    std::unique_ptr<Game> game)
{
    const auto& specs = game->getApplicationSpecifications();

    auto loop = std::make_unique<EngineLoop>(std::move(game));
    Game* gamePtr = loop->getGame();

    std::unique_ptr<IBackendContext> backendContext{};
    IBackendContext* contextPtr = nullptr;

    if (specs.renderBackend != RenderBackend::SDL)
    {
        throw std::runtime_error("Invalid render backend specified");
    }

    
    if (specs.renderBackend == RenderBackend::SDL)
    {
        backendContext = std::make_unique<SDLBackendContext>();
        contextPtr = backendContext.get();
        loop->setClockFunction([]() { return SDL_GetTicks() / 1000.0; });
        loop->setBackendContext(std::move(backendContext));
    }

    if (hasFlag(specs.engineSystem, EngineSystem::Renderer))
    {
        if (contextPtr != nullptr)
        {
            auto sdlRenderer = std::make_unique<SDLRenderer>(*contextPtr);
            sdlRenderer->open(specs.windowOptions);
            loop->addSystem(std::make_unique<RenderSystem>(std::move(sdlRenderer)));
        }
    }

    if (hasFlag(specs.engineSystem, EngineSystem::Audio))
    {
        if (contextPtr != nullptr)
        {
            loop->addSystem(std::make_unique<AudioSystem>(std::make_unique<AudioBackendSDL>()));
        }
    }

    if (hasFlag(specs.engineSystem, EngineSystem::Physics))
    {
        loop->addSystem(std::make_unique<PhysicsSystem>());
    }

    auto sceneManager = std::make_unique<SceneManager>();
    std::unique_ptr<Scene> scenePtr = gamePtr->getFirstScene();
    const std::string sceneName = scenePtr->getName();
    sceneManager->addScene(std::move(scenePtr));
    sceneManager->setActiveScene(sceneName);
    loop->addSystem(std::move(sceneManager));

    if (hasFlag(specs.engineSystem, EngineSystem::NetClient))
    {
        loop->addSystem(std::make_unique<Client>(std::make_unique<TransportGNS>()));
    }

    if (hasFlag(specs.engineSystem, EngineSystem::NetServer))
    {
        loop->addSystem(std::make_unique<Server>(
            ServerConnectionInformation{ specs.networkingOptions.port, specs.networkingOptions.serverIP,},
            std::make_unique<TransportGNS>()));
    }

    return loop;
}
