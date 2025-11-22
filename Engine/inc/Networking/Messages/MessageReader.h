#pragma once
#include <memory>

#include "IncomingRawMessage.h"
#include "MessageTypes.h"


class RawMessage;
class IMessage;


class MessageReader
{
public:
    static std::unique_ptr<IMessage> readMessage(IncomingRawMessage rawMessage);

private:

    static std::unique_ptr<IMessage> createMessage(MessageTypes messageType);

    static MessageTypes readMessageHeader(const IncomingRawMessage& message);
};
