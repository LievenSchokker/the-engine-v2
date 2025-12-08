#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"

class ConnectionMessageHandler : public BaseMessageHandler<ConnectionMessage>
{
    public:
        explicit ConnectionMessageHandler(ConnectionMode mode, GameWorld& gameWorld)
            : BaseMessageHandler( gameWorld) {};

        ~ConnectionMessageHandler() override;

        void handleMessageInternal() override;
};
