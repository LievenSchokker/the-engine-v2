#pragma once
#include <cstdint>

enum class ConnectionStatus : uint8_t
{
    Connected = 0,
    Connecting = 1,
    Disconnected = 2,
    Error = 3,
    Invalid = 4,
    Unknown = 5,
    Death = 6,
};

