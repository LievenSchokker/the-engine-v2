//
// Created by thijs on 14-11-2025.
//


#pragma once
#include <cstdint>
#include "SendMode.h"
#include <string>


struct RawMessage {
    int connectionId = -1;
    std::vector<std::byte> payload;
    SendMode mode = SendMode::ReliableOrdered;
    uint16_t messageType = 0;
    size_t length = 0;

    RawMessage() = default;

    // Construct from raw bytes
    RawMessage(int cid, const std::byte* data, size_t len, SendMode m = SendMode::ReliableOrdered)
        : connectionId(cid), payload(data, data + len), mode(m), length(len) {}

    // Construct from std::string
    RawMessage(int cid, const std::string& str, SendMode m = SendMode::ReliableOrdered)
        : connectionId(cid), payload(reinterpret_cast<const std::byte*>(str.data()),
                                     reinterpret_cast<const std::byte*>(str.data()) + str.size()),
          mode(m), length(str.size()) {}

    // Convert payload to string
    std::string toString() const {
        return std::string(reinterpret_cast<const char*>(payload.data()), payload.size());
    }
};
