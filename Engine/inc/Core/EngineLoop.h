#include "ApplicationSpecifications.h"
#include "GameWorld.h"



class EngineLoop
{
    using ClockFunction = std::function<double()>;
public:
    EngineLoop(ApplicationSpecifications applicationSpecifications);

    virtual ~EngineLoop() = default;

    virtual GameWorld* getGameWorld() = 0;
    virtual SceneManager* getSceneManager() = 0;
    virtual ClockFunction getClock() = 0;
    virtual void start() = 0;
    virtual void update() = 0;
    virtual void fixedUpdate() = 0;
    virtual void shutdown() = 0;
};
