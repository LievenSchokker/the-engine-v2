//
// Created by thijs on 18-11-2025.
//

#pragma once
#include "Transport.h"
#include "TransportGNS.h"

/**
 * @brief Represents a network client that communicates with a server using TransportGNS.
 *
 * Provides functionality to connect to a server, send messages, and handle
 * connection or message callbacks.
 */
class Client
{
public:
    /**
     * @brief Construct a new Client object
     *
     * @param transport Optional TransportGNS instance. If nullptr, a default transport will be created.
     */
    Client(std::unique_ptr<Transport> transport = nullptr);

    /**
     * @brief Destroy the Client object
     *
     * Stops the client, closes the connection, and joins any running threads.
     */
    ~Client();

    /**
     * @brief Set a custom callback for connection state changes.
     *
     * @param newCallback The callback to invoke when connection changes.
     */
    void SetOnConnectionChanged(const OnConnectionChangedCallback& newCallback) const;

    /**
     * @brief Sets the default connection changed callback.
     *
     * Updates the `connected` and `running` flags and prints connection status to stdout.
     */
    void SetDefaultOnConnectionChanged();

    /**
     * @brief Sets the default callback to handle incoming messages.
     *
     * The default behavior is to print received messages to stdout.
     */
    void setDefaultOnMessageReceived() const;

    /**
     * @brief Sets a custom callback for incoming messages.
     *
     * @param newCallback The callback to invoke when a message is received.
     */
    void setOnMessageReceived(const OnMessageReceivedCallback& newCallback) const;

    /**
     * @brief Connects to a server at the given IP and port.
     *
     * @param port Port number of the server.
     * @param serverIP IP address of the server as a C-string.
     * @return true if connection initiation succeeds.
     * @return false if connection initiation fails.
     */
    bool connectToServer(uint16_t port, const char* serverIP);

    /**
     * @brief Sends a message to the server.
     *
     * @param text The message text to send.
     * @return true if the client is connected and the message was sent.
     * @return false if the client is not connected.
     */
    bool sendMessage(const std::string& text);

    bool isConnected() const { return connected; }
    void setConnected(bool state) { connected = state; }

    bool isRunning() const { return running; }
    void setRunning(bool state) { running = state; }

private:
    /**
     * @brief Creates and starts the listen thread to poll the transport for messages.
     */
    void createListenThread();

    std::unique_ptr<Transport> transport; /**< Transport layer used to communicate with the server. */
    std::atomic<bool> connected = false; /**< Whether the client is currently connected. */
    std::atomic<bool> running = false;   /**< Whether the client listen thread is running. */
    int clientConnectionId = -1; /**< Assigned client connection ID by the server. */
    std::thread listenThread;    /**< Thread for polling incoming messages from the server. */
};
