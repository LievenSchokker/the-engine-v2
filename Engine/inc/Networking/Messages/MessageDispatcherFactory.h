

#pragma once

#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Context/NetworkContext.h"
#include "Scene/SceneManager.h"

class IMessage;
class IMessageHandler;


namespace spelmotor_networking
{
    class MessageDispatcherFactory
    {
        public:
            static std::unique_ptr<MessageDispatcher> createMessageDispatcher(ConnectionMode mode, GameWorld& context);
    };
} // spelmotor_networking
