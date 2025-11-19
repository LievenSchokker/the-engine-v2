#pragma once
#include <cstdint>

enum class ConnectionStatus : uint8_t
{
    Connected = 0,
    Disconnected = 1,
    Error = 2,
};

