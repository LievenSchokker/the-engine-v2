#pragma once
#include <cstdint>
#include <string_view>

struct IncomingRawMessage
{
    IncomingRawMessage(int connectionI, const std::byte* dataPointer, size_t length)
        : connectionID(connectionI),
          data(dataPointer),
          length(length)
    {
    }

    int connectionID;
    const std::byte* data;
    size_t length;
};
