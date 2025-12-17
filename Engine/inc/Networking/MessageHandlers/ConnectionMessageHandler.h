#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/ConcreteMessages/ConnectionMessage.h"


class ConnectionMessageHandler final: public BaseMessageHandler<ConnectionMessage>
{
public:
    explicit ConnectionMessageHandler(ConnectionMode mode, GameWorld& gameWorld)
        : BaseMessageHandler( gameWorld) {};

    ~ConnectionMessageHandler() override;

    void handleMessageInternal() override;
};