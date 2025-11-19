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

class TransportGNS : public Transport
{
public:
    TransportGNS();
    ~TransportGNS() override;

    TransportResult setUpListenSocket(uint16_t port) override;
    TransportResult connectByIPAdress(const char* socketAddress, uint16_t port) override;

    TransportResult send(const OutgoingRawMessage& message) override;
    TransportResult sendToAll(const OutgoingRawMessage& message) override;

    bool closeOpenSocket() override;
    bool disconnectFromSocket(int connectionId) override;
    void poll() override;

private:
    void pollConnectionStateChanges();
    void pollIncomingMessages();
    static void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* info);
    void onSteamNetConnectionStatusChanged(const SteamNetConnectionStatusChangedCallback_t* info);

    int getConnectionId(HSteamNetConnection steamConn);
    HSteamNetConnection getSteamConnection(int connectionId);
    std::vector<int> getActiveConnectionIds();
    static int getSendFlags(SendMode sendMode);

    HSteamListenSocket listenSocket;
    HSteamNetPollGroup pollGroup;
    ISteamNetworkingSockets* steamNetworkingSockets;

    std::map<HSteamNetConnection, int> mapConnections;
    std::mutex mapMutex;
    int nextConnectionId;
    static TransportGNS* pCallbackInstance;
};