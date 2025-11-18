///
/// Created by thijs on 12-11-2025.
///


#pragma once
#include <map>
#include <mutex>
#include <steam/steamnetworkingsockets.h>
#include "Transport.h"


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
class TransportGNS : public Transport {
public:
    /**
     * Constructs a new TransportGNS object and initializes GameNetworkingSockets.
     */
    TransportGNS();


    /**
     * @brief Destroys the TransportGNS object and shuts down the GNS system.
     */
    ~TransportGNS() override;


    /**
     * @brief Starts a GNS socket listening on the given port.
     * @param port The port to listen on.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult setUpListenSocket(uint16_t port) override;


    /**
     * @brief Connects to a remote GNS socket.
     * @param socketAddress The socket's IP address (IPv4 or IPv6).
     * @param port The socket's port.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult connectByIPAdress(const char *socketAddress, uint16_t port) override;

    /**
     * @brief Sends a message over the network.
     *
     * @param connectionId The logical connection ID (for clients, usually 0).
     * @param data Pointer to the message buffer.
     * @param length Length of the message buffer in bytes.
     * @param send_mode The desired transmission mode.
     * @return A TransportResult indicating success or failure.
     */
    TransportResult send(const RawMessage& raw_message) override;


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
    TransportResult sendToAll(const RawMessage& raw_message) override;


    /**
     * @brief Closes connection to a remote socket.
     * @param connectionId The ID of the connection to close.
     * @return True if successfully disconnected, false otherwise.
     */
    bool disconnectFromSocket(int connectionId) override;


    /**
     * @brief Polls for incoming messages and connection state changes.
     */
    void poll() override;


    /**
     * @brief Shuts down the socket and closes all active connections.
    */
    bool closeOpenSocket() override;
private:
    void chooseSendFlags(const SendMode sendMode, int& sendFlags);
    /// @name Server Members
    /// @{
    HSteamListenSocket listenSocket;                   /// Listening socket for server mode.
    HSteamNetPollGroup pollGroup;                      /// Poll group for managing multiple connections.
    std::map<HSteamNetConnection, int> mapConnections; /// Maps Steam connections to internal connection IDs.
    std::mutex mapMutex;                               /// Thread-safe access to mapConnections
    std::vector<int> getActiveConnectionIds();         /// Get all connection ids which are active
    /// @}

    /// @name Common
    /// @{
    ISteamNetworkingSockets *steamNetworkingSockets;    /// Pointer to the main GNS interface.
    int nextConnectionId;                               /// Incremental ID for new incoming connections.
    /// @}


    /// @name Callbacks
    /// @{
    static TransportGNS *pCallbackInstance;      /// Static instance pointer for static callbacks.
    static void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo);
    void onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo);
    /// @}


    /// @name Helper Methods
    /// @{
    void pollIncomingMessages();                 /// Processes all queued incoming messages.
    void pollConnectionStateChanges();           /// Handles connection state change events.
    int getConnectionId(HSteamNetConnection hConn); /// Retrieves internal ID for a connection handle.
    HSteamNetConnection getSteamConnection(int connectionId); /// Retrieves connection handle from ID.
    /// @}
};
