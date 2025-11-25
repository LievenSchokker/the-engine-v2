//
// Created by samle on 25/11/2025.
//


#pragma once
namespace spelmotor_networking
{
    class MessageDispatcher;
}

class IMessage;
class IMessageHandler;


namespace spelmotor_networking
{
    class MessageDispatcherFactory
    {
        public:
            static unique_ptr<MessageDispatcher> createMessageDispatcher();
    };
} // spelmotor_networking
