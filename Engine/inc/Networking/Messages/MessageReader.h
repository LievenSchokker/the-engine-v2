#pragma once
#include <memory>


class RawMessage;
class IMessage;


class MessageReader
{
public:
    static std::unique_ptr<IMessage> readMessage(const RawMessage& message);

    static uint16_t readMessageType(const RawMessage& message);
};
