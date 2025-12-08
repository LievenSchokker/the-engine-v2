//
// Created by samle on 24/11/2025.
//

#pragma once


class IMessage;

/**
* @brief Interface for handling network messages (@c IMessage).
 *
 * This interface should be used for any class that wants to handle an instance of IMessage by executing some behaviour or logic.
 * */
class IMessageHandler
{
    public:
        virtual ~IMessageHandler() = default;
        virtual void handleMessage(const IMessage& message) = 0;
};
