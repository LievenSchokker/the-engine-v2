#pragma once
#include <memory>

#include "IncommingRawMessage.h"
#include "MessageTypes.h"


class RawMessage;
class IMessage;


class MessageReader
{
public:
    static std::unique_ptr<IMessage> readMessage(IncomingRawMessage rawMessage);
    static MessageTypes readMessageType(const IncomingRawMessage& message);

private:
    static std::unique_ptr<IMessage> createMessage(IncomingRawMessage rawMessage);
};
