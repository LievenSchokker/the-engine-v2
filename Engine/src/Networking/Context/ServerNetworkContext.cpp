//
// Created by samle on 25/11/2025.
//
#include "../../../inc/Networking/Context/ServerNetworkContext.h"

#include <random>

#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "GameObject/Vector2.h"
#include "Scene/SceneManager.h"

ServerNetworkContext::~ServerNetworkContext() = default;


bool ServerNetworkContext::spawnGameObject(uint32_t netId, Vector2 position)
{
    /// How to determine if object can be spawned can?

    return sceneManager->getActiveScene()->addGameObject(std::make_unique<GameObject>());
}

bool ServerNetworkContext::destroyGameObject(uint32_t netId)
{
    /// How to get the GameObject we need?

    auto go = sceneManager->getActiveScene()->getGameObject("GameObject"); /// <-Temp; use netId to get the GameObject we need.
    return false;
}

bool ServerNetworkContext::sendRPC()
{

    return false;
}

bool ServerNetworkContext::sendCommand()
{
    return false;

}




