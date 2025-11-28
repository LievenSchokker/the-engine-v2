#pragma once


#include <cstdint>


enum class MessageTypes : uint8_t
{
    ConnectionMessage = 1,
    ActionMessage = 2,
    SpawnMessage = 3,
    ObjectDestroyMessage = 4,
    WelcomeMessage = 5,
	StateSyncMessage = 6,
};
