#pragma once

#include <memory>
#include <thread>

#include "ServerInformation.h"
#include "core/Networking/NetworkID.h"
#include "../Connection/ConnectionManager.h"
#include "core/Networking/Transport.h"

/**
 * @brief Manages server-side network connections and client communication.
 */
class Server
{
public:
    /**
     * @brief Constructs a server with the given connection information.
     * @param serverConnectionInformation Configuration for the server connection.
     */
    Server(const ServerConnectionInformation & serverConnectionInformation);

    ~Server();

    /**
     * @brief Starts the server.
     * @return The status of the server after attempting to start.
     */
    ServerStatus start();

    /**
     * @brief Stops the server.
     * @return The status of the server after stopping.
     */
    ServerStatus stop();

    /**
     * @brief Handles incoming messages from clients.
     * @param networkId The network ID of the client sending the message.
     * @param data Pointer to the message data.
     * @param length Length of the message in bytes.
     */
    void onMessage(RawMessage message);

    /**
     * @brief Updates the server state (call regularly in main loop).
     */
    void update();

    /**
     * @brief Disconnects a client from the server.
     * @param clientId The network ID of the client to disconnect.
     */
    void kickClient(NetworkId clientId);

private:
    ServerConnectionInformation setupInformation;
    ServerStatus status;

    std::unique_ptr<ConnectionManager> connectionManager;
    std::vector<NetworkId> clients;
};