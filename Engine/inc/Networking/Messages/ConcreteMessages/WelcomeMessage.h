#pragma once

#include "Networking/Messages/IMessage.h"
#include "Networking/Serialization/Serialization.h"

class WelcomeMessage : public IMessage
{
public:
    WelcomeMessage() = default;
    explicit WelcomeMessage(int clientId) : assignedClientId(clientId) {}

    std::vector<std::byte> serialize() const override
    {
        WriteArchive archive;
        archive.process(const_cast<int&>(assignedClientId));
        return archive.getBytes();
    }

    bool deserialize(const std::byte* data, size_t length) override
    {
        if (!data || length == 0) return false;
        ReadArchive archive(data, length);
        archive.process(assignedClientId);
        return true;
    }

    bool validate() const override { return assignedClientId > 0; }
    MessageTypes getMessageType() const override { return MessageTypes::WelcomeMessage; }

    int getClientId() const { return assignedClientId; }

private:
    int assignedClientId = 0;
};