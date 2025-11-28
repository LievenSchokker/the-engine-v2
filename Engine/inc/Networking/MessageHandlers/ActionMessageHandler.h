#pragma once

#include "BaseMessageHandler.h"
#include "Networking/Messages/Concretes/ActionMessage.h"

class NetworkContext;
class NetworkIdentityRegistry;

class ActionMessageHandler: public BaseMessageHandler<ActionMessage>
{
public:
	ActionMessageHandler(NetworkContext& context,
	                     NetworkIdentityRegistry& registry);
	~ActionMessageHandler() override = default;

protected:
	void handleMessageInternal(const ActionMessage& message) override;

private:
	NetworkIdentityRegistry& identityRegistry;
};