//
// Created by samle on 25/11/2025.
//


#pragma once
#include <cstdint>
class SceneManager;
class ISceneStrategy;
struct Vector2;

/// NOTE: This struct can be used to inject Application dependencies to the MessageHandlers.
/// Add more if neeced
struct  NetworkContext
{
    public:
        explicit NetworkContext(SceneManager& scenesManager) : sceneManager(&scenesManager) {}
        virtual ~NetworkContext() = default;

        SceneManager& getSceneManager() const {return  *sceneManager; }

    protected:
        SceneManager* sceneManager;
};
