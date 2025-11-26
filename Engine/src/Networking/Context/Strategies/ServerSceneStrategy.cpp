//
// Created by samle on 26/11/2025.
//
#include "../../../../inc/Networking/Context/Strategies/ServerSceneStrategy.h"

#include "Scene/SceneManager.h"


void ServerSceneStrategy::loadScene()
{
    sceneManager->loadScene("NewScene");
}

bool ServerSceneStrategy::spawnGameObject()
{
    return sceneManager->getActiveScene()->addGameObject(std::make_unique<GameObject>());
}


bool ServerSceneStrategy::destroyGameObject(const std::string& name)
{
    return sceneManager->getActiveScene()->removeGameObject(name);
}


void ServerSceneStrategy::injectDependencies(SceneManager& sceneManager_)
{
    sceneManager = &sceneManager_;
}