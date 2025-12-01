#pragma once

/**
 * @brief Specifies the role of a network participant.
 */
enum class ConnectionMode
{
    Host,   ///< Acts as the server, accepting incoming connections.
    Client  ///< Acts as a client, connecting to a host.
};