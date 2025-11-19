///
/// Created by thijs on 12-11-2025.
///


#include "Networking/TransportGNS.h"
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
    SteamDatagramErrMsg errorMessage;
    if (!GameNetworkingSockets_Init(nullptr, errorMessage))
    {
        std::cerr << "GameNetworkingSockets_Init failed: " << errorMessage << std::endl;
        return;
    }

    steamNetworkingSockets = SteamNetworkingSockets();

    // Set debug output
    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Msg,
        [](ESteamNetworkingSocketsDebugOutputType eType, const char* message)
        {
            std::cout << message << std::endl;
        }
    );

    // Global connection state callback
    SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(
        steamNetConnectionStatusChangedCallback
    );
}


TransportGNS::~TransportGNS()
{
    TransportGNS::closeOpenSocket();
    GameNetworkingSockets_Kill();
}


TransportResult TransportGNS::setUpListenSocket(const uint16_t port)
{

    SteamNetworkingIPAddr address{};
    address.Clear();
    address.m_port = port;


    // Create a listening socket
    listenSocket = steamNetworkingSockets->CreateListenSocketIP(address, 0, nullptr);
    if (listenSocket == k_HSteamListenSocket_Invalid)
    {
        std::cerr << "[GNS] Failed to listen on port " << port << std::endl;
        return TransportResult::ERROR;
    }

    // Create a poll group for managing multiple connections
    pollGroup = steamNetworkingSockets->CreatePollGroup();
    if (pollGroup == k_HSteamNetPollGroup_Invalid)
    {
        std::cerr << "[GNS] Failed to create poll group\n";
        return TransportResult::ERROR;
    }

    std::cout << "[GNS] Listening on port " << port << "\n";
    return TransportResult::SUCCES;
}


TransportResult TransportGNS::connectByIPAdress(const char* socketAddress, uint16_t port)
{
    SteamNetworkingIPAddr address{};
    if (!address.ParseString(socketAddress))
    {
        std::cerr << "[GNS] Invalid address: " << socketAddress << '\n';
        return TransportResult::ERROR;
    }
    address.m_port = port;

    HSteamNetConnection steamNetworkConnection = steamNetworkingSockets->ConnectByIPAddress(address, 0, nullptr);

    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
    {
        std::cerr << "[GNS] ConnectByIPAddress failed\n";
        return TransportResult::ERROR;
    }

    return TransportResult::SUCCES;
}


TransportResult TransportGNS::send(const RawMessage& message)
{
    int sendFlags = 0;
    chooseSendFlags(message.getSendMode(), sendFlags);

    HSteamNetConnection steamNetworkConnection = getSteamConnection(message.getConnectionID());
    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
    {
        return TransportResult::ERROR;
    }

    std::vector<std::byte> payload = message.getPayload();
    EResult result = steamNetworkingSockets->SendMessageToConnection(
        steamNetworkConnection,
        payload.data(),
        static_cast<uint32>(payload.size()),
        sendFlags,
        nullptr
    );

    if (result == k_EResultOK)
    {
        return TransportResult::SUCCES;
    }

    return TransportResult::ERROR;
}


TransportResult TransportGNS::sendToAll(const RawMessage& message)
{
    for (int connectionID : getActiveConnectionIds())
    {
        RawMessage clone = message;
        clone.setConnectionID(connectionID);

        TransportResult transportResult = send(clone);
        if (transportResult == TransportResult::ERROR)
        {
            return TransportResult::ERROR;
        }
    }
    return TransportResult::SUCCES;
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
    HSteamNetConnection steamNetworkConnection = getSteamConnection(connectionId);

    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
        return false;

    steamNetworkingSockets->CloseConnection(steamNetworkConnection, 0, "Disconnected", true);

    std::lock_guard<std::mutex> lock(mapMutex);
    mapConnections.erase(steamNetworkConnection);
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
            ISteamNetworkingMessage* message = nullptr;
            int number = steamNetworkingSockets->ReceiveMessagesOnPollGroup(pollGroup, &message, 1);
            if (number != 1) break;

            int cid = getConnectionId(message->m_conn);
            if (cid != -1 && onMessageReceived)
            {
                RawMessage rm(cid,
                              (const std::byte*)message->m_pData,
                              message->m_cbSize);
                onMessageReceived(rm);
            }
            message->Release();
        }
    }

    // 2. Poll outbound or P2P connections
    for (auto& [steamNetworkConnection, connectionId] : mapConnections)
    {
        ISteamNetworkingMessage* message = nullptr;
        while (steamNetworkingSockets->ReceiveMessagesOnConnection(steamNetworkConnection, &message, 1) == 1)
        {
            RawMessage rawMessage(connectionId,
                          (const std::byte*)message->m_pData,
                          message->m_cbSize);

            if (onMessageReceived)
                onMessageReceived(rawMessage);

            message->Release();
        }
    }
}


void TransportGNS::steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    pCallbackInstance->onSteamNetConnectionStatusChanged(pointerConnectionStatusInformation);
}


void TransportGNS::onSteamNetConnectionStatusChanged(const SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    switch (pointerConnectionStatusInformation->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_Connecting:
        {
            // Incoming connection?
            if (pointerConnectionStatusInformation->m_info.m_hListenSocket == listenSocket &&
                listenSocket != k_HSteamListenSocket_Invalid)
            {
                int connectionId;
                {
                    std::lock_guard<std::mutex> lock(mapMutex);
                    connectionId = nextConnectionId++;
                    mapConnections[pointerConnectionStatusInformation->m_hConn] = connectionId;
                }

                if (steamNetworkingSockets->AcceptConnection(pointerConnectionStatusInformation->m_hConn) != k_EResultOK)
                {
                    steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr, false);
                    return;
                }

                if (!steamNetworkingSockets->SetConnectionPollGroup(pointerConnectionStatusInformation->m_hConn, pollGroup))
                {
                    steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr, false);
                    return;
                }
            }
            break;
        }

    case k_ESteamNetworkingConnectionState_Connected:
        {
            int connectionId;
            {
                std::lock_guard<std::mutex> lock(mapMutex);
                connectionId = nextConnectionId++;
                mapConnections[pointerConnectionStatusInformation->m_hConn] = connectionId;
            }

            if (onConnectionChanged)
                onConnectionChanged(connectionId, true);

            break;
        }

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            int connectionId;
            {
                std::lock_guard<std::mutex> lock(mapMutex);
                connectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
                if (connectionId != -1)
                    mapConnections.erase(pointerConnectionStatusInformation->m_hConn);
            }

            if (connectionId != -1 && onConnectionChanged)
                onConnectionChanged(connectionId, false);

            steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr, false);

            break;
        }

    default:
        break;
    }
}


int TransportGNS::getConnectionId(HSteamNetConnection steamNetworkConnection)
{
    auto it = mapConnections.find(steamNetworkConnection);
    if (it != mapConnections.end())
        return it->second;
    return -1;
}


HSteamNetConnection TransportGNS::getSteamConnection(int connectionId)
{
    for (const auto& connectionPair : mapConnections)
    {
        if (connectionPair.second == connectionId)
            return connectionPair.first;
    }
    return k_HSteamNetConnection_Invalid;
}


std::vector<int> TransportGNS::getActiveConnectionIds()
{
    std::lock_guard<std::mutex> lock(mapMutex);
    std::vector<int> ids;
    for (auto& connectionPair : mapConnections)
        ids.push_back(connectionPair.second);
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
