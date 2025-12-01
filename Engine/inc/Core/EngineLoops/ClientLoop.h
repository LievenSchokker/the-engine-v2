#pragma once


#include "ApplicationSpecifications.h"
#include "GameWorld.h"
#include "Core/EngineLoop.h"

#include <functional>
#include <memory>

#include "External/SdlContext.h"

class ClientLoop: public EngineLoop
{
    using ClockFunction = std::function<double()>;
public:
    explicit ClientLoop(ApplicationSpecifications applicationSpecifications, GameWorld* gameWorld);

    ~ClientLoop() override = default;

    GameWorld* getGameWorld() override;
    SceneManager* getSceneManager() override;
    ClockFunction getClock() override;
    void start() override;
    void initializeNetworking();
    void update() override;
    void fixedUpdate() override;
    void shutdown() override;

private:
    ApplicationSpecifications applicationSpecifications;
    std::unique_ptr<GameWorld> gameWorld;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<Client> client;
    std::unique_ptr<IPhysicsWorld> physicsWorld;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<ClockFunction> clock;
};
