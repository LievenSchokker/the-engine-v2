//
// Created by samle on 26/11/2025.
//

#pragma once


class SceneManager;
class GameObject;

#include <string>


class ISceneStrategy
{
    public:
        virtual ~ISceneStrategy() = default;
        virtual void loadScene() = 0;
        virtual bool spawnGameObject() = 0;
        virtual bool destroyGameObject(const std::string& name) = 0;

        /// This should go in the constructor, but for now here so we can compile without setting all correct references during development.
        virtual void injectDependencies(SceneManager& sceneManager);
};

