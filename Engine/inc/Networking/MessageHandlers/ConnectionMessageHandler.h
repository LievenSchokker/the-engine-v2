#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"

class ConnectionMessageHandler : public BaseMessageHandler<ConnectionMessage>
{
    public:
        explicit ConnectionMessageHandler(ConnectionMode mode, NetworkContext& networkContext)
            : BaseMessageHandler( networkContext) {};

        ~ConnectionMessageHandler() override;

        void handleMessageInternal(const ConnectionMessage &message) override;
};
