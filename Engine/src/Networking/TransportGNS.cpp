#include "Networking/TransportGNS.h"
#include <iostream>
#include <mutex>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

#include "Networking/SendMode.h"
#include "Networking/Messages/IncommingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"

TransportGNS* TransportGNS::pCallbackInstance = nullptr;

TransportGNS::TransportGNS()
    : listenSocket(k_HSteamListenSocket_Invalid)
      , pollGroup(k_HSteamNetPollGroup_Invalid)
      , steamNetworkingSockets(nullptr)
      , nextConnectionId(1)
{
    SteamDatagramErrMsg errorMessage;
    if (!GameNetworkingSockets_Init(nullptr, errorMessage))
    {
        return;
    }

    steamNetworkingSockets = SteamNetworkingSockets();

    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Msg,
        [](ESteamNetworkingSocketsDebugOutputType eType, const char* message)
        {
            std::cout << message << std::endl;
        }
    );

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

    listenSocket = steamNetworkingSockets->CreateListenSocketIP(address, 0, nullptr);
    if (listenSocket == k_HSteamListenSocket_Invalid)
    {
        return TransportResult::ERROR;
    }

    pollGroup = steamNetworkingSockets->CreatePollGroup();
    if (pollGroup == k_HSteamNetPollGroup_Invalid)
    {
        return TransportResult::ERROR;
    }

    return TransportResult::SUCCES;
}

TransportResult TransportGNS::connectByIPAdress(const char* socketAddress, uint16_t port)
{
    SteamNetworkingIPAddr address{};
    if (!address.ParseString(socketAddress))
    {
        return TransportResult::ERROR;
    }
    address.m_port = port;

    HSteamNetConnection steamNetworkConnection = steamNetworkingSockets->ConnectByIPAddress(address, 0, nullptr);

    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
    {
        return TransportResult::ERROR;
    }

    return TransportResult::SUCCES;
}

TransportResult TransportGNS::send(const OutgoingRawMessage& message)
{
    int sendFlags = getSendFlags(message.sendMode);

    HSteamNetConnection steamNetworkConnection = getSteamConnection(message.connectionID);
    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
    {
        return TransportResult::ERROR;
    }

    EResult result = steamNetworkingSockets->SendMessageToConnection(
        steamNetworkConnection,
        message.data(),
        static_cast<uint32>(message.size()),
        sendFlags,
        nullptr
    );

    return (result == k_EResultOK) ? TransportResult::SUCCES : TransportResult::ERROR;
}

TransportResult TransportGNS::sendToAll(const OutgoingRawMessage& message)
{
    std::vector<int> connectionIds = getActiveConnectionIds();

    for (int connectionID : connectionIds)
    {
        std::vector<std::byte> bufferCopy = message.buffer;

        OutgoingRawMessage messageCopy
            (connectionID, std::move(bufferCopy), message.sendMode);

        TransportResult result = send(messageCopy);
        if (result == TransportResult::ERROR)
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
        steamNetworkingSockets->
            CloseConnection(pair.first, 0, "Close open socket", true);
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

    steamNetworkingSockets->
        CloseConnection(steamNetworkConnection, 0, "Disconnected", true);

    std::lock_guard<std::mutex> lock(mapMutex);
    mapConnections.erase(steamNetworkConnection);
    return true;
}

void TransportGNS::poll()
{
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
    if (pollGroup != k_HSteamNetPollGroup_Invalid)
    {
        while (true)
        {
            ISteamNetworkingMessage* steamMessage = nullptr;
            int number = steamNetworkingSockets->ReceiveMessagesOnPollGroup(pollGroup, &steamMessage, 1);
            if (number != 1) break;

            int cid = getConnectionId(steamMessage->m_conn);
            if (cid != -1 && onMessageReceived)
            {
                IncomingRawMessage incoming(
                    cid,
                    (const std::byte*)steamMessage->m_pData,
                    steamMessage->m_cbSize
                );
                onMessageReceived(incoming);
            }
            steamMessage->Release();
        }
    }

    std::vector<std::pair<HSteamNetConnection, int>> connectionsCopy;
    {
        std::lock_guard<std::mutex> lock(mapMutex);
        for (const auto& pair : mapConnections)
            connectionsCopy.push_back(pair);
    }

    for (auto& [steamNetworkConnection, connectionId] : connectionsCopy)
    {
        ISteamNetworkingMessage* steamMessage = nullptr;
        while (steamNetworkingSockets->ReceiveMessagesOnConnection(steamNetworkConnection, &steamMessage, 1) == 1)
        {
            IncomingRawMessage incoming(
                connectionId,
                (const std::byte*)steamMessage->m_pData,
                steamMessage->m_cbSize
            );

            if (onMessageReceived)
                onMessageReceived(incoming);

            steamMessage->Release();
        }
    }
}

void TransportGNS::steamNetConnectionStatusChangedCallback(
    SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    pCallbackInstance->onSteamNetConnectionStatusChanged(pointerConnectionStatusInformation);
}


void TransportGNS::onSteamNetConnectionStatusChanged(
    const SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    switch (pointerConnectionStatusInformation->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_Connecting:
        {
            if (pointerConnectionStatusInformation->m_info.m_hListenSocket == listenSocket &&
                listenSocket != k_HSteamListenSocket_Invalid)
            {
                if (steamNetworkingSockets->AcceptConnection(pointerConnectionStatusInformation->m_hConn) !=
                    k_EResultOK)
                {
                    steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr,
                                                            false);
                    return;
                }

                if (!steamNetworkingSockets->SetConnectionPollGroup(pointerConnectionStatusInformation->m_hConn,
                                                                    pollGroup))
                {
                    steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr,
                                                            false);
                    return;
                }

                {
                    std::lock_guard<std::mutex> lock(mapMutex);
                    int connectionId = nextConnectionId++;
                    mapConnections[pointerConnectionStatusInformation->m_hConn] = connectionId;
                }

            }
            break;
        }

    case k_ESteamNetworkingConnectionState_Connected:
        {
            int connectionId;
            {
                std::lock_guard<std::mutex> lock(mapMutex);

                auto it = mapConnections.find(pointerConnectionStatusInformation->m_hConn);
                if (it != mapConnections.end())
                {
                    connectionId = it->second;
                }
                else
                {
                    connectionId = nextConnectionId++;
                    mapConnections[pointerConnectionStatusInformation->m_hConn] = connectionId;
                }
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

            if (connectionId != -1)
            {
                if (onConnectionChanged)
                    onConnectionChanged(connectionId, false);
            }

            steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr, false);

            break;
        }

    default:
        break;
    }
}

int TransportGNS::getConnectionId(HSteamNetConnection steamNetworkConnection)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = mapConnections.find(steamNetworkConnection);
    if (it != mapConnections.end())
        return it->second;
    return -1;
}

HSteamNetConnection TransportGNS::getSteamConnection(int connectionId)
{
    std::lock_guard<std::mutex> lock(mapMutex);
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

int TransportGNS::getSendFlags(SendMode sendMode)
{
    switch (sendMode)
    {
    case SendMode::ReliableOrdered:
        return k_nSteamNetworkingSend_Reliable;
    case SendMode::ReliableUnordered:
        return k_nSteamNetworkingSend_Reliable | k_nSteamNetworkingSend_NoDelay;
    case SendMode::Unreliable:
        return k_nSteamNetworkingSend_Unreliable;
    default:
        return k_nSteamNetworkingSend_Reliable;
    }
}
