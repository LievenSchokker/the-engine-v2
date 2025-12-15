#pragma once


struct Connection;

#include "TransportResult.h"
#include "Messages/IncomingRawMessage.h"

#include <functional>


class OutgoingRawMessage;
/**
 * @typedef OnMessageReceivedCallback
 * @brief Callback invoked when a raw network message is received.
 *
 * Triggered whenever the transport layer delivers an incoming message.
 * The callback receives an ::IncomingRawMessage.
 *
 * @warning The underlying data pointer is only valid for the duration
 *          of the callback. Do not store it beyond this scope.
 *
 * @param message The received message.
 */
using OnMessageReceivedCallback = std::function<void(const IncomingRawMessage&)>;


/**
 * @typedef OnConnectionChangedCallback
 * @brief Callback invoked when a connection is created, updated, or closed.
 *
 * Triggered when the transport layer reports a change in connection state.
 * The callback receives a ::Connection describing the updated state.
 *
 * @param connection The connection whose state has changed.
 */
using OnConnectionChangedCallback = std::function<void(Connection connection)>;

/**
 * @class ITransport
 * @brief Abstract base class for network transport layers.
 *
 * Provides an interface for server/client networking, sending messages,
 * and managing connections. Derived classes implement specific networking
 * APIs such as GameNetworkingSockets, ENet, or custom protocols.
 */
class ITransport
{
public:
    virtual ~ITransport() = default;


    /**
     * @brief Starts a server listening on a given port.
     * @param port The port to bind and listen on.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult setUpListenSocket(const uint16_t& port) = 0;


    /**
     * @brief Connects to a server at a given address and port.
     * @param serverAddress IP address or hostname of the server.
     * @param port Server port.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult connectByIPAdress(const char* serverAddress, const uint16_t& port) = 0;


    /**
     * @brief Sends a message over the network.
     * @param RawMessage The message to send over the network.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult send(const OutgoingRawMessage& message) = 0;


    /**
     * @brief Sends a message over the network to all active nodes
     * @param RawMessage The message to send over the network to all connections.
     * @return A TransportResult indicating success or failure.
     */
    virtual TransportResult sendToAll(OutgoingRawMessage& message) = 0;


    /**
     * @brief Disconnects a connection.
     * @param connectionId The connection ID to disconnect.
     * @return True if successful, false otherwise.
     */
    virtual bool disconnectFromSocket(const int& connectionId) = 0;


    /**
     * @brief Polls the network for messages and connection events.
     */
    virtual void poll() = 0;


    /**
     * @brief Shuts down the transport, closing all connections and cleaning up resources.
     */
    virtual bool closeOpenSocket() = 0;


    /**
     * @brief Sets the callback for message reception.
     * @param callback A callable to invoke when a message is received.
     */
    void setOnMessageReceived(const OnMessageReceivedCallback& callback) { onMessageReceived = callback; }


    /**
     * @brief Sets the callback for connection state changes.
     * @param callback A callable to invoke when a connection is established or closed.
     */
    void setOnConnectionChanged(const OnConnectionChangedCallback& callback) { onConnectionChanged = callback; }

protected:
    OnMessageReceivedCallback onMessageReceived; ///< Callback for received messages.
    OnConnectionChangedCallback onConnectionChanged; ///< Callback for connection changes.
};
