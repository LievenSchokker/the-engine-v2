//
// Created by samle on 24/11/2025.
//

#pragma once


#include <cstdint>
#include <memory>
#include <unordered_map>


enum class MessageTypes : uint8_t;
class IMessage;
class IMessageHandler;


namespace spelmotor_networking
{
    class MessageDispatcher
    {
        public:
            virtual ~MessageDispatcher() = default;
            void processMessage(const IMessage& message);
            bool registerMessageHandler(MessageTypes type, std::unique_ptr<IMessageHandler> handler);
            bool deregisterMessageHandler(MessageTypes type);

        private:
            IMessageHandler* getMessageHandler(MessageTypes type);
            std::unordered_map<MessageTypes, std::unique_ptr<IMessageHandler>> messageHandlers;
    };
}
