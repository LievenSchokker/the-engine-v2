#pragma once

#include "Core/IEngineLoop.h"
#include "External/IBackendContext.h"

#include <functional>
#include <memory>
#include <vector>

class IEngineSystem;
class Game;
class GameWorld;
class SceneManager;
class ApplicationClock;
struct ApplicationSpecifications;

class EngineLoop : public IEngineLoop
{
public:
    using ClockFunction = std::function<double()>;

    explicit EngineLoop(std::unique_ptr<Game> game);
    ~EngineLoop() override;

    void addSystem(std::unique_ptr<IEngineSystem> system);

    template <typename T>
    T* getSystem();
    void start() override;
    void update(double deltaTime) override;
    void fixedUpdate(double deltaTime) override;
    void shutdown() override;
    [[nodiscard]] bool isShutdownRequested() const override;

    ClockFunction getClock() override;
    GameWorld* getGameWorld() override;
    SceneManager* getSceneManager() override;
    [[nodiscard]] Game* getGame() const;
    [[nodiscard]] ApplicationSpecifications getSpecifications() const;
    [[nodiscard]] IBackendContext* getBackendContext() const;
    const std::vector<std::unique_ptr<IEngineSystem>>& getSystems() const;

    void setApplicationClock(ApplicationClock* clock) override;
    void setClockFunction(ClockFunction func);
    void setBackendContext(std::unique_ptr<IBackendContext> context);
    void requestShutdown();

private:
    std::unique_ptr<IBackendContext> backendContext;
    std::unique_ptr<Game> game;
    std::unique_ptr<GameWorld> gameWorld;
    std::vector<std::unique_ptr<IEngineSystem>> systems;

    ClockFunction clockFunction;
    bool shutdownRequested = false;

    SceneManager* sceneManagerPtr = nullptr;
};