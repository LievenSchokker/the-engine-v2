///
/// Created by thijs on 12-11-2025.
///

#include "engine/networking/TransportGNS.h"
#include <iostream>
#include <mutex>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>


TransportGNS* TransportGNS::pCallbackInstance = nullptr;


TransportGNS::TransportGNS()
    : listenSocket(k_HSteamListenSocket_Invalid)
      , pollGroup(k_HSteamNetPollGroup_Invalid)
      , steamNetworkingSockets(nullptr)
      , nextConnectionId(1)
{
    // Initialize GameNetworkingSockets
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg))
    {
        std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << std::endl;
        return;
    }

    steamNetworkingSockets = SteamNetworkingSockets();

    // Set debug output
    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Msg,
        [](ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
        {
            std::cout << pszMsg << std::endl;
        }
    );

    // Global connection state callback
    SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(
        steamNetConnectionStatusChangedCallback
    );
}


TransportGNS::~TransportGNS()
{
    closeOpenSocket();
    GameNetworkingSockets_Kill();
}


TransportResult TransportGNS::setUpListenSocket(uint16_t port)
{
    TransportResult result;

    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.m_port = port;


    // Create a listening socket
    listenSocket = steamNetworkingSockets->CreateListenSocketIP(addr, 0, nullptr);
    if (listenSocket == k_HSteamListenSocket_Invalid)
    {
        std::cerr << "[GNS] Failed to listen on port " << port << std::endl;
        result.success = false;
        return result;
    }

    // Create a poll group for managing multiple connections
    pollGroup = steamNetworkingSockets->CreatePollGroup();
    if (pollGroup == k_HSteamNetPollGroup_Invalid)
    {
        std::cerr << "[GNS] Failed to create poll group\n";
        result.success = false;
        return result;
    }

    std::cout << "[GNS] Listening on port " << port << "\n";
    result.success = true;
    return result;
}


TransportResult TransportGNS::connectByIPAdress(const char* socketAddress, uint16_t port)
{
    TransportResult result;

    SteamNetworkingIPAddr addr;
    if (!addr.ParseString(socketAddress))
    {
        std::cerr << "[GNS] Invalid address: " << socketAddress << '\n';
        result.success = false;
        return result;
    }
    addr.m_port = port;

    HSteamNetConnection hConn = steamNetworkingSockets->ConnectByIPAddress(addr, 0, nullptr);

    if (hConn == k_HSteamNetConnection_Invalid)
    {
        std::cerr << "[GNS] ConnectByIPAddress failed\n";
        result.success = false;
        return result;
    }

    result.success = true;
    return result;
}


TransportResult TransportGNS::send(const RawMessage& message)
{
    TransportResult result;

    int sendFlags = 0;
    chooseSendFlags(message.getSendMode(), sendFlags);

    HSteamNetConnection hConn = getSteamConnection(message.getConnectionID());
    if (hConn == k_HSteamNetConnection_Invalid)
    {
        result.success = false;
        return result;
    }

    std::vector<std::byte> payload = message.getPayload();
    EResult r = steamNetworkingSockets->SendMessageToConnection(
        hConn,
        payload.data(),
        static_cast<uint32>(payload.size()),
        sendFlags,
        nullptr
    );

    result.success = (r == k_EResultOK);
    return result;
}


TransportResult TransportGNS::sendToAll(const RawMessage& message)
{
    TransportResult result;
    result.success = true;

    for (int connectionID : getActiveConnectionIds())
    {
        RawMessage clone = message;
        clone.setConnectionID(connectionID);

        TransportResult transportResult = send(clone);
        if (!transportResult.success)
        {
            result.success = false;
        }
    }
    return result;
}


bool TransportGNS::closeOpenSocket()
{
    for (auto& pair : mapConnections)
    {
        steamNetworkingSockets->CloseConnection(pair.first, 0, "Close open socket", true);
    }
    mapConnections.clear();

    if (listenSocket != k_HSteamListenSocket_Invalid)
    {
        steamNetworkingSockets->CloseListenSocket(listenSocket);
        listenSocket = k_HSteamListenSocket_Invalid;
    }

    if (pollGroup != k_HSteamNetPollGroup_Invalid)
    {
        steamNetworkingSockets->DestroyPollGroup(pollGroup);
        pollGroup = k_HSteamNetPollGroup_Invalid;
    }

    return true;
}


bool TransportGNS::disconnectFromSocket(int connectionId)
{
    HSteamNetConnection hConn = getSteamConnection(connectionId);

    if (hConn == k_HSteamNetConnection_Invalid)
        return false;

    steamNetworkingSockets->CloseConnection(hConn, 0, "Disconnected", true);

    std::lock_guard<std::mutex> lock(mapMutex);
    mapConnections.erase(hConn);
    return true;
}


void TransportGNS::poll()
{
    // Handle both connection state changes and queued messages
    pollConnectionStateChanges();
    pollIncomingMessages();
}


void TransportGNS::pollConnectionStateChanges()
{
    pCallbackInstance = this;
    steamNetworkingSockets->RunCallbacks();
}


void TransportGNS::pollIncomingMessages()
{
    // 1. Poll group (incoming accepted connections)
    if (pollGroup != k_HSteamNetPollGroup_Invalid)
    {
        while (true)
        {
            ISteamNetworkingMessage* msg = nullptr;
            int num = steamNetworkingSockets->ReceiveMessagesOnPollGroup(pollGroup, &msg, 1);
            if (num != 1) break;

            int cid = getConnectionId(msg->m_conn);
            if (cid != -1 && onMessageReceived)
            {
                RawMessage rm(cid,
                              (const std::byte*)msg->m_pData,
                              msg->m_cbSize);
                onMessageReceived(rm);
            }
            msg->Release();
        }
    }

    // 2. Poll outbound or P2P connections
    for (auto& [hConn, cid] : mapConnections)
    {
        ISteamNetworkingMessage* msg = nullptr;
        while (steamNetworkingSockets->ReceiveMessagesOnConnection(hConn, &msg, 1) == 1)
        {
            RawMessage rm(cid,
                          (const std::byte*)msg->m_pData,
                          msg->m_cbSize);

            if (onMessageReceived)
                onMessageReceived(rm);

            msg->Release();
        }
    }
}


void TransportGNS::steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    pCallbackInstance->onSteamNetConnectionStatusChanged(pInfo);
}


void TransportGNS::onSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    switch (pInfo->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_Connecting:
        {
            // Incoming connection?
            if (pInfo->m_info.m_hListenSocket == listenSocket &&
                listenSocket != k_HSteamListenSocket_Invalid)
            {
                int connId;
                {
                    std::lock_guard<std::mutex> lock(mapMutex);
                    connId = nextConnectionId++;
                    mapConnections[pInfo->m_hConn] = connId;
                }

                if (steamNetworkingSockets->AcceptConnection(pInfo->m_hConn) != k_EResultOK)
                {
                    steamNetworkingSockets->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    return;
                }

                if (!steamNetworkingSockets->SetConnectionPollGroup(pInfo->m_hConn, pollGroup))
                {
                    steamNetworkingSockets->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                    return;
                }
            }
            break;
        }

    case k_ESteamNetworkingConnectionState_Connected:
        {
            int cid;
            {
                std::lock_guard<std::mutex> lock(mapMutex);
                cid = nextConnectionId++;
                mapConnections[pInfo->m_hConn] = cid;
            }

            if (onConnectionChanged)
                onConnectionChanged(cid, true);

            break;
        }

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            int cid;
            {
                std::lock_guard<std::mutex> lock(mapMutex);
                cid = getConnectionId(pInfo->m_hConn);
                if (cid != -1)
                    mapConnections.erase(pInfo->m_hConn);
            }

            if (cid != -1 && onConnectionChanged)
                onConnectionChanged(cid, false);

            steamNetworkingSockets->CloseConnection(pInfo->m_hConn, 0, nullptr, false);

            break;
        }

    default:
        break;
    }
}


int TransportGNS::getConnectionId(HSteamNetConnection hConn)
{
    auto it = mapConnections.find(hConn);
    if (it != mapConnections.end())
        return it->second;
    return -1;
}


HSteamNetConnection TransportGNS::getSteamConnection(int connectionId)
{
    for (const auto& pair : mapConnections)
    {
        if (pair.second == connectionId)
            return pair.first;
    }
    return k_HSteamNetConnection_Invalid;
}


std::vector<int> TransportGNS::getActiveConnectionIds()
{
    std::lock_guard<std::mutex> lock(mapMutex);
    std::vector<int> ids;
    for (auto& pair : mapConnections)
        ids.push_back(pair.second);
    return ids;
}


void TransportGNS::chooseSendFlags(const SendMode sendMode, int& sendFlags)
{
    switch (sendMode)
    {
    case SendMode::ReliableOrdered:
        sendFlags = k_nSteamNetworkingSend_Reliable;
        break;
    case SendMode::ReliableUnordered:
        sendFlags = k_nSteamNetworkingSend_Reliable | k_nSteamNetworkingSend_UnreliableNoDelay;
        break;
    case SendMode::Unreliable:
        sendFlags = k_nSteamNetworkingSend_Unreliable;
        break;
    }
}
