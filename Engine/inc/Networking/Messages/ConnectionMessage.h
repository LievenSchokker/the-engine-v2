#pragma once
#include "IMessage.h"
#include "Networking/Connection/Connection.h"


class ConnectionMessage : IMessage
{
public:
    ConnectionMessage();

    std::vector<std::byte> serialize() const override;
    bool deserialize(const std::byte* data, size_t length) override;
    bool validate() const override;


private:
    ConnectionStatus status;
};
