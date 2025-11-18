#pragma once


#include "ApplicationSpecifications.h"

class IRender;

class SpelMotor
{
public:
    SpelMotor(ApplicationSpecifications applicationSpecifications);

    void run();
    void shutdown();
    void requestShutdown();


private:
    void update();
    void processShutdown();

    bool running;
    bool shouldShutdown;

    const ApplicationSpecifications specifications;

    int frameCounter;
    float frameTime;
    float lastFrameTime;
    float timeStep;

    std::unique_ptr<IRender> renderer;
};
