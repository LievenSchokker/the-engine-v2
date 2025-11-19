#pragma once
#include <memory>

#include "MessageTypes.h"


class RawMessage;
class IMessage;


class MessageReader
{
public:
    static std::unique_ptr<IMessage> readMessage(const RawMessage message);
    static MessageTypes readMessageType(const RawMessage& message);

private:
    static std::unique_ptr<IMessage> createMessage(RawMessage rawMessage);
};
