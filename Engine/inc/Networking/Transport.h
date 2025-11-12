#pragma once
#include <cstddef>
#include <functional>
#include "TransportResult.h"
#include "SendMode.h"

/**
 * @typedef OnMessageReceivedCallback
 * @brief Callback invoked when a message is received.
 * @param connectionId ID of the connection that sent the message.
 * @param data Pointer to the received message data.
 * @param length Length of the received message in bytes.
 */
using OnMessageReceivedCallback = std::function<void(int connectionId, const std::byte *data, size_t length)>;

/**
 * @typedef OnConnectionChangedCallback
 * @brief Callback invoked when a connection is established or closed.
 * @param connectionId ID of the connection.
 * @param connected True if connection is now active, false if disconnected.
 */
using OnConnectionChangedCallback = std::function<void(int connectionId, bool connected)>;

/**
 * @class Transport
 * @brief Abstract base class for network transport layers.
 *
 * Provides an interface for server/client networking, sending messages,
 * and managing connections. Derived classes implement specific networking
 * APIs such as GameNetworkingSockets, ENet, or custom protocols.
 */
class Transport {
public:
    virtual ~Transport() = default;


    /**
     * @brief Starts a server listening on a given port.
     * @param port The port to bind and listen on.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult startServer(uint16_t port) = 0;


    /**
     * @brief Connects to a server at a given address and port.
     * @param serverAddress IP address or hostname of the server.
     * @param port Server port.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult startClient(const char *serverAddress, uint16_t port) = 0;


    /**
     * @brief Sends a message over the network.
     * @param connectionId The connection to send to (0 for single-client mode).
     * @param data Pointer to the message buffer.
     * @param length Length of the message in bytes.
     * @param send_mode The desired reliability and ordering (SendMode).
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult send(int connectionId, const std::byte data[], size_t length, SendMode send_mode) = 0;


    /**
     * @brief Disconnects a connection.
     * @param connectionId The connection ID to disconnect.
     * @return True if successful, false otherwise.
     */
    virtual bool disconnect(int connectionId) = 0;


    /**
     * @brief Polls the network for messages and connection events.
     */
    virtual void poll() = 0;


    /**
     * @brief Shuts down the transport, closing all connections and cleaning up resources.
     */
    virtual void shutdown() = 0;


    /**
     * @brief Sets the callback for message reception.
     * @param callback A callable to invoke when a message is received.
     */
    void setOnMessageReceived(OnMessageReceivedCallback callback) { onMessageReceived = callback; }


    /**
     * @brief Sets the callback for connection state changes.
     * @param callback A callable to invoke when a connection is established or closed.
     */
    void setOnConnectionChanged(OnConnectionChangedCallback callback) { onConnectionChanged = callback; }

protected:
    OnMessageReceivedCallback onMessageReceived; ///< Callback for received messages.
    OnConnectionChangedCallback onConnectionChanged; ///< Callback for connection changes.
};
