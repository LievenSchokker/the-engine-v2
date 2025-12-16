#pragma once


#include "BaseMessageHandler.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

class ServerWelcomeMessageHandler final : public BaseMessageHandler<WelcomeMessage>
{
public:
    explicit ServerWelcomeMessageHandler(GameWorld& world, NetworkSpawnManager& spawnManager);
    ~ServerWelcomeMessageHandler() override = default;

protected:
    void handleMessageInternal() override;

private:
    NetworkSpawnManager& spawnManager;
};