#pragma once


#include "ConnectionStatus.h"
#include "ConnectionMode.h"


/**
 * @brief Represents a network connection and its current state.
 *
 * Holds identifying information and status for a single connection,
 * whether acting as a host or client.
 */
struct Connection
{
    ConnectionMode mode;              ///< The role of this connection (host or client).
    int transportConnectionId;        ///< Unique identifier assigned by the transport layer.
    ConnectionStatus connectionStatus; ///< Current state of the connection.
};