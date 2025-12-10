#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/ConcreteMessages/ConnectionMessage.h"


class ConnectionMessageHandler: public BaseMessageHandler<ConnectionMessage>
{
<<<<<<< HEAD
public:
	explicit ConnectionMessageHandler(ConnectionMode mode,
	                                  NetworkContext& networkContext)
		: BaseMessageHandler(networkContext)
	{
	};
=======
    public:
        explicit ConnectionMessageHandler(ConnectionMode mode, GameWorld& gameWorld)
            : BaseMessageHandler( gameWorld) {};
>>>>>>> origin/development

	~ConnectionMessageHandler() override;

<<<<<<< HEAD
	void handleMessageInternal(const ConnectionMessage& message) override;
};
=======
        void handleMessageInternal() override;
};
>>>>>>> origin/development
