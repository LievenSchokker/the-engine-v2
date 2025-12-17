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
public:
    virtual ~IMessageHandler() = default;
    virtual void handleMessage(std::unique_ptr<IMessage> message) = 0;
};