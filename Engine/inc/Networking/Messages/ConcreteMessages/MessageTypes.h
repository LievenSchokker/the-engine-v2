#pragma once


#include <cstdint>


enum class MessageTypes : uint8_t
{
    ConnectionMessage = 0,
	ActionMessage = 1,
};
