//
// Created by samle on 24/11/2025.
//
#pragma once
#include "IMessageHandler.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Context/INetworkContext.h"
#include <type_traits>
/**
 * Abstract base class for all concrete MessageHandlers.
 *
 * Template ensures an IMessage is always associated with the correct handler behaviour corresponding to it.
 *
 * Use this base class when implementing new IMessage types and requiring some behaviour to handle them.
 * @tparam TMessage the IMessage this handler works on.
 */
template<typename TMessage>
class BaseMessageHandler : public IMessageHandler
{
    static_assert(std::is_base_of<IMessage, TMessage>::value,
                  "[BaseMessageHandler]: TMessage must derive from IMessage.");

    public:
        explicit BaseMessageHandler(ConnectionMode mode, const INetworkContext& networkContext_) : connectionMode(mode), networkContext(networkContext_) {}
        ~BaseMessageHandler() override = default;

        void handleMessage(const IMessage &message) override
        {
            const TMessage *concreteMessage = dynamic_cast<const TMessage *>(&message);
            if (concreteMessage != nullptr)
            {
                /// A)
                switch (connectionMode)
                {
                    case ConnectionMode::Client:
                        handleMessageClient(*concreteMessage);
                    case ConnectionMode::Host:
                        handleMessageServer(*concreteMessage);
                }

                /// B)

            }
        }

    protected:
        virtual void handleMessage(const TMessage& message) = 0; /// B
        virtual void handleMessageClient(const TMessage &message) = 0; /// A
        virtual void handleMessageServer(const TMessage &message) = 0; /// A

        ConnectionMode connectionMode; /// A
        const INetworkContext& networkContext; /// B
};
