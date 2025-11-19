#pragma once
#include <vector>
#include <cstdint>
#include "Networking/SendMode.h"

class OutgoingRawMessage
{
public:
    OutgoingRawMessage(int connectionID, std::vector<std::byte> payload,
        SendMode mode = SendMode::ReliableOrdered)
        : connectionID(connectionID)
          , buffer(std::move(payload))
          , sendMode(mode)
    {
    }

    int connectionID;
    std::vector<std::byte> buffer;
    SendMode sendMode;

    const std::byte* data() const { return buffer.data(); }
    size_t size() const { return buffer.size(); }
};
