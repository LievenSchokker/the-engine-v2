//
// Created by samle on 25/11/2025.
//
#include "../../../inc/Networking/MessageHandlers/SpawnGameObjectMessageHandler.h"

#include "Networking/Context/Strategies/ISceneStrategy.h"

SpawnGameObjectMessageHandler::~SpawnGameObjectMessageHandler() = default;


void SpawnGameObjectMessageHandler::handleMessageContextBased(const spelmotor_networking::SpawnGameObjectMessage &message)
{

    networkContext.getSceneStrategy().spawnGameObject();
}

void SpawnGameObjectMessageHandler::handleMessageServer(const spelmotor_networking::SpawnGameObjectMessage &message)
{

}

void SpawnGameObjectMessageHandler::handleMessageClient(const spelmotor_networking::SpawnGameObjectMessage &message)
{

}


