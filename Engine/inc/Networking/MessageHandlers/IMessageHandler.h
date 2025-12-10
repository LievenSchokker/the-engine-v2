#pragma once


#include <memory>

class IMessage;


/**
 * @brief Interface for handling network messages (@c IMessage).
 *
 * This interface should be used for any class that wants to handle an instance
 * of IMessage by executing some behaviour or logic.
 * */
class IMessageHandler
{
<<<<<<< HEAD
public:
	virtual ~IMessageHandler() = default;
	virtual void handle(const IMessage& message) = 0;
};
=======
   public:
	virtual ~IMessageHandler() = default;
	virtual void handleMessage(std::unique_ptr<IMessage> message) = 0;
};
>>>>>>> origin/development
