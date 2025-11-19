#pragma once
#include <cstdint>
#include <string_view>

struct IncomingRawMessage
{
    IncomingRawMessage(int connID, const std::byte* ptr, size_t len)
        : connectionID(connID), data(ptr), length(len)
    {
    }

    int connectionID;
    const std::byte* data;
    size_t length;
};
