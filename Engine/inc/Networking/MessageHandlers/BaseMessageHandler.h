#pragma once


#include "Core/GameWorld.h"
#include "IMessageHandler.h"

#include <type_traits>


/**
 * Abstract base class for all concrete MessageHandlers.
 *
 * Template ensures an IMessage is always associated with the correct handler
 * behaviour corresponding to it.
 *
 * Use this base class when implementing new IMessage types and requiring some
 * behaviour to handle them.
 * @tparam ConcreteTemplateMessage the IMessage this handler works on.
 */
<<<<<<< HEAD
template <typename TMessage>
class BaseMessageHandler: public IMessageHandler
{
	static_assert(std::is_base_of<IMessage, TMessage>::value,
	              "TMessage must derive from IMessage.");

public:
	explicit
	BaseMessageHandler(NetworkContext& networkContext_) : networkContext(
		networkContext_)
=======
template <typename ConcreteTemplateMessage>
class BaseMessageHandler: public IMessageHandler
{
	static_assert(std::is_base_of<IMessage, ConcreteTemplateMessage>::value,
				  "[BaseMessageHandler]: TMessage must derive from IMessage.");

   public:
	explicit BaseMessageHandler(GameWorld& gameWorld) : gameWorld(&gameWorld)
>>>>>>> origin/development
	{
	}

	~BaseMessageHandler() override = default;

<<<<<<< HEAD
	void handle(const IMessage& message) override
	{
		auto* concrete = dynamic_cast<const TMessage*>(&message);

		if (concrete != nullptr)
		{
			handleMessageInternal(*concrete);
		}
	}

protected:
	virtual void handleMessageInternal(const TMessage& message) = 0;

	NetworkContext& networkContext;
};
=======
	void handleMessage(const std::unique_ptr<IMessage> message) override
	{
		auto* concrete = dynamic_cast<ConcreteTemplateMessage*>(message.get());

		if ( concrete != nullptr )
		{
			internalMessage =
				std::make_unique<ConcreteTemplateMessage>(*concrete);
			handleMessageInternal();
		}
	}

   protected:
	virtual void handleMessageInternal() = 0;
	GameWorld* gameWorld;

   private:
	std::unique_ptr<ConcreteTemplateMessage> internalMessage;
};
>>>>>>> origin/development
