//
// Created by samle on 26/11/2025.
//

#pragma once

#include "ISceneStrategy.h"

class ServerSceneStrategy : public ISceneStrategy
{
    public:
        ServerSceneStrategy() = default;
        ~ServerSceneStrategy() override = default;

        void loadScene() override;
        bool spawnGameObject() override;
        bool destroyGameObject(const std::string& name) override;

        void injectDependencies(SceneManager& sceneManager_) override;

    private:
        SceneManager* sceneManager;
};
