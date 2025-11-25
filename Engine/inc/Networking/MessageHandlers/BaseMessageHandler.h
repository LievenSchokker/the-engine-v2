    //
    // Created by samle on 24/11/2025.
    //

    #pragma once


    #include "IMessageHandler.h"
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
        static_assert(std::is_base_of<IMessage, TMessage>::value, "[BaseMessageHandler]: TMessage must derive from IMessage.");

        public:
            virtual ~BaseMessageHandler() = default;

            void handleMessage(const IMessage& message) override
            {
                const TMessage* concreteMessage = dynamic_cast<const TMessage*>(&message);
                if (concreteMessage != nullptr)
                {
                    handleMessage(*concreteMessage);
                }
            }

        protected:
            virtual void handleMessage(const TMessage& message) = 0;
    };
