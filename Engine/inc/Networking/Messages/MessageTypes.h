#pragma once


#include <cstdint>


enum class MessageTypes : uint8_t
{
    ConnectionMessage = 0,
    ActionMessage = 1,
    SpawnMessage = 2,
    ObjectDestroyMessage = 3,
    WelcomeMessage = 4
};
