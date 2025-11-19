// ConnectionMessage.h
#pragma once
#include "IMessage.h"
#include "MessageTypes.h"
#include "Networking/Connection/ConnectionStatus.h"

class ConnectionMessage : public IMessage
{
public:
    ConnectionMessage();

    MessageTypes getMessageType() const override
    {
        return MessageTypes::ConnectionMessage;
    }

    std::vector<std::byte> serialize() const override;
    bool deserialize(const std::byte* data, size_t length) override;
    bool validate() const override;

    ConnectionStatus getStatus() const;
    void setStatus(ConnectionStatus newStatus);

private:
    ConnectionStatus status;
};