//
// Created by samle on 25/11/2025.
//
#include "../../../inc/Networking/Context/ClientNetworkContext.h"

#include "GameObject/Vector2.h"

ClientNetworkContext::~ClientNetworkContext() = default;


bool ClientNetworkContext::spawnGameObject(uint32_t netId, Vector2 position)
{
    return false;
}

bool ClientNetworkContext::destroyGameObject(uint32_t netId)
{
    return false;

}

bool ClientNetworkContext::sendRPC()
{
    return false;

}

bool ClientNetworkContext::sendCommand()
{
    return false;
}