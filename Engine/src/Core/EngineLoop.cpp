#include "Core/EngineLoop.h"
#include "Core/Options/ApplicationSpecifications.h"
#include "Core/GameWorld.h"
#include "Core/IEngineSystems.h"
#include "Game.h"
#include "Input/InputManager.h"

#include <ostream>

EngineLoop::EngineLoop(std::unique_ptr<Game> game)
    : game(std::move(game))
    , gameWorld(std::make_unique<GameWorld>())
    , clockFunction([]() { return 0.0; })
{
}

EngineLoop::~EngineLoop() = default;

ApplicationSpecifications EngineLoop::getSpecifications() const
{
    return game->getApplicationSpecifications();
}

void EngineLoop::addSystem(std::unique_ptr<IEngineSystem> system)
{
    systems.push_back(std::move(system));
}

void EngineLoop::start()
{
    for (const auto& system : systems)
    {
        system->start(*gameWorld);
    }

    getGameWorld()->input = InputManager::getInstance();
    sceneManagerPtr = gameWorld->sceneManager;
}

void EngineLoop::update(const double deltaTime)
{
    for (const auto& system : systems)
    {
        system->update(deltaTime, *gameWorld);
    }

    getGameWorld()->input->update();
}

void EngineLoop::fixedUpdate(const double deltaTime)
{
    for (const auto& system : systems)
    {
        system->fixedUpdate(deltaTime, *gameWorld);
    }
}

void EngineLoop::shutdown()
{
    for (auto it = systems.rbegin(); it != systems.rend(); ++it)
    {
        (*it)->shutdown(*gameWorld);
    }
    systems.clear();
}

bool EngineLoop::isShutdownRequested() const
{
    return shutdownRequested;
}

EngineLoop::ClockFunction EngineLoop::getClock()
{
    return clockFunction;
}

GameWorld* EngineLoop::getGameWorld()
{
    return gameWorld.get();
}

SceneManager* EngineLoop::getSceneManager()
{
    return sceneManagerPtr;
}

Game* EngineLoop::getGame() const
{
    return game.get();
}

IBackendContext* EngineLoop::getBackendContext() const
{
    return backendContext.get();
}

const std::vector<std::unique_ptr<IEngineSystem>>& EngineLoop::getSystems() const
{
    return systems;
}

void EngineLoop::setApplicationClock(ApplicationClock* clock)
{
    if (gameWorld)
    {
        gameWorld->clock = clock;
    }
}

void EngineLoop::setClockFunction(ClockFunction func)
{
    clockFunction = std::move(func);
}

void EngineLoop::setBackendContext(std::unique_ptr<IBackendContext> context)
{
    backendContext = std::move(context);
}

void EngineLoop::requestShutdown()
{
    shutdownRequested = true;
}