// TransportGNS.h
#pragma once
#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <steam/steamtypes.h>

#include "Transport.h"  // ← ADD THIS
#include "TransportResult.h"

class OutgoingRawMessage;
struct IncomingRawMessage;
enum class SendMode;
class ISteamNetworkingSockets;
struct SteamNetConnectionStatusChangedCallback_t;
typedef uint32 HSteamNetConnection;
typedef uint32 HSteamListenSocket;
typedef uint32 HSteamNetPollGroup;

/**
 * @class TransportGNS
 * @brief Implements a transport layer using Valve's GameNetworkingSockets (GNS).
 *
 * This class provides both opening socket and sending data to sockets functionality.
 * Using the SteamNetworkingSockets API.
 *
 * @note Both reliable and unreliable send modes are supported. ReliableUnordered
 * mode behaves similarly to reliable ordered, but with reduced latency due to
 * disabled message coalescing.
 */
class TransportGNS : public Transport
{
public:
    TransportGNS();
    ~TransportGNS() override;

    /**
     * @brief Starts a GNS socket listening on the given port.
     * @param port The port to listen on.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult setUpListenSocket(const uint16_t& port) override;

    /**
     * @brief Connects to a remote GNS socket.
     * @param socketAddress The socket's IP address (IPv4 or IPv6).
     * @param port The socket's port.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult connectByIPAdress(const char* socketAddress, const uint16_t& port) override;

    /**
     * @brief Sends a message over the network.
     *
     * @param message
     * @param connectionId The logical connection ID (for clients, usually 0).
     * @param data Pointer to the message buffer.
     * @param length Length of the message buffer in bytes.
     * @param send_mode The desired transmission mode.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult send(const OutgoingRawMessage& message) override;


    /**
     * @brief Sends a message over the network to all connected nodes
     *
     * The message is sent using the specified SendMode:
     * - ReliableOrdered: Guarantees in-order delivery.
     * - ReliableUnordered: Reliable but sent with minimal delay (order not guaranteed).
     * - Unreliable: May be dropped or arrive out of order.
     *
     * @param data Pointer to the message buffer.
     * @param length Length of the message buffer in bytes.
     * @param send_mode The desired transmission mode.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult sendToAll(OutgoingRawMessage& message) override;


    /**
     * @brief Shuts down the socket and closes all active connections.
    */
    bool closeOpenSocket() override;

    /**
     * @brief Closes connection to a remote socket.
     * @param connectionId The ID of the connection to close.
     * @return True if successfully disconnected, false otherwise.
     */
    bool disconnectFromSocket(const int& connectionId) override;

    /**
     * @brief Polls for incoming messages and connection state changes.
     */
    void poll() override;

private:
    /// Processes all queued incoming connection state changes.
    void pollConnectionStateChanges() const;

    /// Processes all queued incoming messages.
    void pollIncomingMessages();

    void onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation);

    static void steamNetConnectionStatusChangedCallback(
    SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation);

    /// Retrieves internal ID for a connection handle.
    int getConnectionId(HSteamNetConnection steamConn);

    /// Retrieves connection handle from ID.
    HSteamNetConnection getSteamConnection(int connectionId);

    /// Get all connection ids which are active
    std::vector<int> getActiveConnectionIds();

    static int getSendFlags(SendMode sendMode);


    //Steam provides global C style callback which doesn't support usage of the THIS type callback,
    //Creates a static refrence to the currently active callback
    static TransportGNS* transportGNSCallbackInstance;

    HSteamListenSocket listenSocket;
    HSteamNetPollGroup pollGroup;
    ISteamNetworkingSockets* steamNetworkingSockets;

    std::map<HSteamNetConnection, int> mapConnections;
    std::mutex mapMutex;
    int nextConnectionId;
    static TransportGNS* callbackInstance;
};
