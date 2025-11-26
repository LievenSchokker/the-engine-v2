//
// Created by samle on 26/11/2025.
//
#include "../../../../inc/Networking/Context/Strategies/ClientSceneStrategy.h"

#include <memory>

#include "Scene/SceneManager.h"

void ClientSceneStrategy::loadScene()
{
    sceneManager->loadScene("NewScene");
}

bool ClientSceneStrategy::spawnGameObject()
{
    return sceneManager->getActiveScene()->addGameObject(std::make_unique<GameObject>());
}


bool ClientSceneStrategy::destroyGameObject(const std::string& name)
{
    return sceneManager->getActiveScene()->removeGameObject(name);
}

void ClientSceneStrategy::injectDependencies(SceneManager& sceneManager_)
{
    sceneManager = &sceneManager_;
}