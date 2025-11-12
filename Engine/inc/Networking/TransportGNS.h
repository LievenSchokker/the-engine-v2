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
    bool StartServer(uint16_t port) override;
    bool StartClient(const char *serverAddress, uint16_t port) override;
    bool Send(int connectionId, const std::byte data[], size_t length, bool reliable) override;
    bool Disconnect(int connectionId) override;
    void Poll() override;
    void Shutdown() override;

private:
    /// Server members
    HSteamListenSocket m_hListenSock;
    HSteamNetPollGroup m_hPollGroup;
    std::map<HSteamNetConnection, int> m_mapConnections;

    /// Client members
    HSteamNetConnection m_hConnection;

    /// Common
    ISteamNetworkingSockets *m_pInterface;
    bool m_bIsServer;
    int m_nextConnectionId;

    /// Callbacks
    static TransportGNS *s_pCallbackInstance;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo);
    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo);

    /// Helper methods
    void PollIncomingMessages();
    void PollConnectionStateChanges();
    int GetConnectionId(HSteamNetConnection hConn);
    HSteamNetConnection GetSteamConnection(int connectionId);
};
