#pragma once


#include "IMessageHandler.h"
#include "Core/GameWorld.h"
#include <type_traits>

/**
 * Abstract base class for all concrete MessageHandlers.
 *
 * Template ensures an IMessage is always associated with the correct handler behaviour corresponding to it.
 *
 * Use this base class when implementing new IMessage types and requiring some behaviour to handle them.
 * @tparam ConcreteTemplateMessage the IMessage this handler works on.
 */
template<typename ConcreteTemplateMessage>
class BaseMessageHandler : public IMessageHandler
{
    static_assert(std::is_base_of<IMessage, ConcreteTemplateMessage>::value,
                  "[BaseMessageHandler]: TMessage must derive from IMessage.");

    public:
    explicit BaseMessageHandler(GameWorld& gameWorld) : gameWorld(&gameWorld) {}
    ~BaseMessageHandler() override = default;

        void handleMessage(const std::unique_ptr<IMessage> message) override
        {
            auto* concrete = dynamic_cast<ConcreteTemplateMessage*>(message.get());

            if (concrete != nullptr)
            {
                internalMessage = std::make_unique<ConcreteTemplateMessage>(*concrete);
                handleMessageInternal();
            }
        }

    protected:
        virtual void handleMessageInternal() = 0;
        GameWorld* gameWorld;

    private:
        std::unique_ptr<ConcreteTemplateMessage> internalMessage;
};
