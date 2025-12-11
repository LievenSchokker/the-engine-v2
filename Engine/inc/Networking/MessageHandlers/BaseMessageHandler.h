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
    	// Transferring ownership of the pointer from IMessage to ConcreteTemplateMessage
    	// Without making a copy (Since some message's contain unique_ptr's
    	auto* raw = message.release();
    	auto* concrete = dynamic_cast<ConcreteTemplateMessage*>(raw);

    	if (concrete != nullptr)
    	{
    		internalMessage.reset(concrete);
    		handleMessageInternal();
    	}
    	else
    	{
    		delete raw;
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