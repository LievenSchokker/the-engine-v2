#pragma once

namespace spelmotorNetworking
{
    class MessageDispatcher;
}

#include "Connection/Connection.h"
#include "Core/GameWorld.h"
#include "Core/IEngineSystems.h"
#include "Server/ServerInformation.h"
#include "Networking/Messages/MessageDispatcher.h"
#include <memory>

class ITransport;
class TransportGNS;
class IMessage;
struct IncomingRawMessage;

/**
 * @brief Network client for connecting to and communicating with a server.
 *
 * Manages a single server connection, handling message sending/receiving
 * and connection state changes through the underlying transport layer.
 */
class Client : public IEngineSystems
{
public:
    /**
     * @brief Constructs a Client and initializes the transport layer.
     */
    explicit Client(std::unique_ptr<ITransport> transport);

    /**
     * @brief Destructor. Cleans up the transport and any active connection.
     */
    ~Client();

    /**
     * @brief Initiates a connection to a server.
     * @param serverInformartion The server port to connect to.
     * @param serverIP The server IP address as a null-terminated string.
     * @return True if the connection attempt was initiated successfully,
     *         false otherwise.
     */
    bool connectToServer(const ServerConnectionInformation& serverInformartion) const;
    /**
     * @brief Disconnects from the server.
     */
    void disconnect();

	SystemStatus start(GameWorld& gameWorld) override;
    /**
     * @brief Sends a message to the connected server.
     * @param message The message to send.
     * @return True if the message was queued successfully, false otherwise.
     */
    bool sendMessage(const IMessage& message) const;

    /**
     * @brief Processes incoming network events.
     *
     * Should be called regularly (e.g., each frame) to handle incoming
     * messages and connection state changes.
     */
    void update(double deltaTime, const GameWorld& gameWorld) override;

    /**
     * @brief Checks if the client is currently connected to a server.
     * @return True if connected, false otherwise.
     */
    bool isConnected() const;

    void injectMessageDispatcher(
        std::unique_ptr<spelmotorNetworking::MessageDispatcher> dispatcher);


    void shutdown(GameWorld& gameWorld) override;
    const std::string getName() const override;

private:
    /**
     * @brief Callback invoked when a message is received from the server.
     * @param rawMessage The incoming raw message data.
     */
    void onMessageReceived(const IncomingRawMessage& rawMessage) const;

    /**
     * @brief Callback invoked when the connection state changes.
     * @param connection The connection whose state changed.
     */
    void onConnectionChanged(const Connection& connection);

    ///< The underlying network transport.
    std::unique_ptr<ITransport> transport;

    ///< The current server connection.
    Connection currentConnection{};
    std::unique_ptr<spelmotorNetworking::MessageDispatcher> messageDispatcher;
};
