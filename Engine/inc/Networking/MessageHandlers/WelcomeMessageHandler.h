#pragma once

#include "Networking/MessageHandlers/IMessageHandler.h"
#include "Networking/Messages/Concretes/WelcomeMessage.h"
#include "Core/GameWorld.h"
#include <iostream>

class WelcomeMessageHandler : public IMessageHandler
{
public:
    explicit WelcomeMessageHandler(GameWorld& world) : gameWorld(world) {}

    void handle(const IMessage& message) override
    {
        const auto& welcome = static_cast<const WelcomeMessage&>(message);
        gameWorld.localClientId = welcome.getClientId();
    }

private:
    GameWorld& gameWorld;
};