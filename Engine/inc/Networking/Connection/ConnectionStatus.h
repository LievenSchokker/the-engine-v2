#pragma once


#include <cstdint>


/**
 * @brief Represents the state of a network connection.
 */
enum class ConnectionStatus : uint8_t
{
    Connected = 0,     ///< Connection is active and ready for communication.
    Connecting = 1,    ///< Connection attempt is in progress.
    Disconnected = 2,  ///< Connection has been cleanly closed.
    Error = 3,         ///< Connection failed due to an error.
    Invalid = 4,       ///< Connection is in an invalid state.
    Unknown = 5,       ///< Connection state could not be determined.
    Terminated = 6  ///< Connection was terminated unexpectedly.
};