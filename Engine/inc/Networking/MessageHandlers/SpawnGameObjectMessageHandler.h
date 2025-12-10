#pragma once


#include "BaseMessageHandler.h"
#include "Networking/Context/NetworkContext.h"


class SpawnGameObjectMessageHandler: public BaseMessageHandler<
		spelmotor_networking::SpawnGameObjectMessage>
{
public:
	explicit
	SpawnGameObjectMessageHandler(NetworkContext& networkContext) :
		BaseMessageHandler(networkContext)
	{
	};
	~SpawnGameObjectMessageHandler() override;

	void handle(const IMessage& message) override;
};