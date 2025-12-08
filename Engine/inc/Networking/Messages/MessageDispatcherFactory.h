#pragma once


#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Scene/SceneManager.h"

class IMessage;
class IMessageHandler;


namespace spelmotor_networking
{
class MessageDispatcherFactory
{
   public:
	static std::unique_ptr<MessageDispatcher> createMessageDispatcher(
		ConnectionMode mode, GameWorld& context);
};
}  // namespace spelmotor_networking
