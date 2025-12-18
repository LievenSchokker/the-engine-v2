#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

class WelcomeMessageHandler : public BaseMessageHandler<WelcomeMessage>
{
public:
    explicit WelcomeMessageHandler(GameWorld& world)
        : BaseMessageHandler<WelcomeMessage>(world) {}

    ~WelcomeMessageHandler() override = default;

protected:
    void handleMessageInternal() override
    {
    	std::cout << "Setting Welcome message to" << getMessage()->getClientId() << std::endl;
        gameWorld->localClientId = getMessage()->getClientId();
    }
};