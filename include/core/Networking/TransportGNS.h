///
/// Created by thijs on 12-11-2025.
///


#include <map>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include "Transport.h"


class TransportGNS : public Transport
{
public:
    TransportGNS();
    ~TransportGNS() override;
    TransportResult startServer(uint16_t port) override;
    TransportResult startClient(const char *serverAddress, uint16_t port) override;
    TransportResult send(int connectionId, const std::byte data[], size_t length, bool reliable) override;
    bool disconnect(int connectionId) override;
    void poll() override;
    void shutdown() override;

private:
    /// Server members
    HSteamListenSocket hListenSock;
    HSteamNetPollGroup hPollGroup;
    std::map<HSteamNetConnection, int> mapConnections;

    /// Client members
    HSteamNetConnection hConnection;

    /// Common
    ISteamNetworkingSockets *pInterface;
    bool bIsServer;
    int nextConnectionId;

    /// Callbacks
    static TransportGNS *pCallbackInstance;
    static void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo);
    void onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo);

    /// Helper methods
    void pollIncomingMessages();
    void pollConnectionStateChanges();
    int getConnectionId(HSteamNetConnection hConn);
    HSteamNetConnection getSteamConnection(int connectionId);
};
