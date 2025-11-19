#pragma once
#include <cstdint>
#include <string_view>

struct IncomingRawMessage
{
    int connectionID;
    const std::byte* data;
    size_t length;

    IncomingRawMessage(int connID, const std::byte* ptr, size_t len)
        : connectionID(connID), data(ptr), length(len)
    {
    }

    std::string_view asStringView() const
    {
        return std::string_view(reinterpret_cast<const char*>(data), length);
    }
};
