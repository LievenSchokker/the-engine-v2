#pragma once

#include "Core/GameWorld.h"
#include "IMessageHandler.h"

#include <type_traits>

template <typename ConcreteTemplateMessage>
class BaseMessageHandler : public IMessageHandler
{
    static_assert(std::is_base_of<IMessage, ConcreteTemplateMessage>::value,
                  "[BaseMessageHandler]: TMessage must derive from IMessage.");

public:
    explicit BaseMessageHandler(GameWorld& gameWorld) : gameWorld(&gameWorld)
    {
    }

    ~BaseMessageHandler() override = default;

    void handleMessage(std::unique_ptr<IMessage> message) override
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

    // Add this getter so derived classes can access the message
    ConcreteTemplateMessage* getMessage() const { return internalMessage.get(); }

    GameWorld* gameWorld;

private:
    std::unique_ptr<ConcreteTemplateMessage> internalMessage;
};