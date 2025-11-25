#include "Networking/Messages/Concretes/ConnectionMessage.h"

#include <cstring>
#include <iostream>

ConnectionMessage::ConnectionMessage()
    : status(ConnectionStatus::Disconnected)
{
}

std::vector<std::byte> ConnectionMessage::serialize() const
{
    std::vector<std::byte> buffer(sizeof(uint8_t));
    auto statusValue = static_cast<uint8_t>(status);
    std::memcpy(buffer.data(), &statusValue, sizeof(uint8_t));

    return buffer;
}

bool ConnectionMessage::deserialize(const std::byte* data, size_t length)
{
    if (length < sizeof(uint8_t))
    {
        return false;
    }

    uint8_t statusValue;
    std::memcpy(&statusValue, data, sizeof(uint8_t));

    if (statusValue > static_cast<uint8_t>(ConnectionStatus::Terminated))
    {
        return false;
    }

    status = static_cast<ConnectionStatus>(statusValue);
    return true;
}

bool ConnectionMessage::validate() const
{
    switch (status)
    {
    case ConnectionStatus::Connected:
    case ConnectionStatus::Disconnected:
    case ConnectionStatus::Error:
        return true;
    default:
        return false;
    }
}

ConnectionStatus ConnectionMessage::getStatus() const
{
    return status;
}

void ConnectionMessage::setStatus(ConnectionStatus newStatus)
{
    status = newStatus;
}