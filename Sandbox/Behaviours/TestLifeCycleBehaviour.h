#pragma once

#include "Behaviour/Behaviour.h"

#include <iostream>
#include <string>

/**
 * @class TestLifecycleBehaviour
 * @brief A behaviour that logs all lifecycle method calls for testing purposes.
 *
 * Use this to verify that behaviours are properly initialized and updated
 * when GameObjects are added to a scene at runtime.
 */
class TestLifecycleBehaviour : public Behaviour
{
public:
    explicit TestLifecycleBehaviour(std::string name = "Unnamed")
        : objectName(std::move(name)), updateCount(0)
    {
        std::cout << "[TestLifecycle:" << objectName << "] Constructor called" << std::endl;
    }

    ~TestLifecycleBehaviour() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] Destructor called" << std::endl;
    }

    void onAwake() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] onAwake()" << std::endl;
    }

    void onEnable() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] onEnable()" << std::endl;
    }

    void onStart() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] onStart()" << std::endl;
    }

    void update(double deltaTime, const GameWorld& gameWorld) override
    {
        (void)deltaTime;
        (void)gameWorld;

        updateCount++;

        if (updateCount <= 3)
        {
            std::cout << "[TestLifecycle:" << objectName << "] update() #" << updateCount << std::endl;
        }
        else if (updateCount == 4)
        {
            std::cout << "[TestLifecycle:" << objectName << "] update() ... (further updates suppressed)" << std::endl;
        }
    }

    void onDisable() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] onDisable() (total updates: " << updateCount << ")" << std::endl;
    }

    void onDestroy() override
    {
        std::cout << "[TestLifecycle:" << objectName << "] onDestroy()" << std::endl;
    }

    int getUpdateCount() const { return updateCount; }
    const std::string& getObjectName() const { return objectName; }

private:
    std::string objectName;
    int updateCount;
};