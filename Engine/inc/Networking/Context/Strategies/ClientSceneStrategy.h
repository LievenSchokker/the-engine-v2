//
// Created by samle on 26/11/2025.
//

#pragma once
#include "ISceneStrategy.h"

class ClientSceneStrategy : public ISceneStrategy
{
    public:
        ClientSceneStrategy() = default;
        ~ClientSceneStrategy() override = default;

        void loadScene() override;
        bool spawnGameObject() override;
        bool destroyGameObject(const std::string &name) override;
        void injectDependencies(SceneManager &sceneManager) override;

    private:
        SceneManager* sceneManager;
};


