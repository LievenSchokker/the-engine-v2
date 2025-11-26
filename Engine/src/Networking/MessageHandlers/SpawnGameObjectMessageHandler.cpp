//
// Created by samle on 25/11/2025.
//
#include "../../../inc/Networking/MessageHandlers/SpawnGameObjectMessageHandler.h"

#include "Networking/Context/Strategies/ISceneStrategy.h"
#include "Scene/SceneManager.h"

SpawnGameObjectMessageHandler::~SpawnGameObjectMessageHandler() = default;


void SpawnGameObjectMessageHandler::handleMessage(const IMessage &message)
{
    /// Use networkContext.getSceneManager() here to add a new object.
}



