#include "Networking/TransportGNS.h"
#include <iostream>
#include <mutex>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

#include "Networking/SendMode.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"


TransportGNS* TransportGNS::transportGNSCallbackInstance = nullptr;

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

    SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(steamNetConnectionStatusChangedCallback);
}

TransportGNS::~TransportGNS()
{
    TransportGNS::closeOpenSocket();
    GameNetworkingSockets_Kill();
}

TransportResult TransportGNS::setUpListenSocket(const uint16_t& port)
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

TransportResult TransportGNS::connectByIPAdress(const char* socketAddress, const uint16_t& port)
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

TransportResult TransportGNS::sendToAll(OutgoingRawMessage& message)
{
    std::vector<int> connectionIds = getActiveConnectionIds();

    for (int connectionID : connectionIds)
    {
        message.connectionID = connectionID;
        TransportResult result = send(message);

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
            CloseConnection(pair.first, 0, "Socket closed by host", true);
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

bool TransportGNS::disconnectFromSocket(const int& connectionId)
{
    HSteamNetConnection steamNetworkConnection = getSteamConnection(connectionId);

    if (steamNetworkConnection == k_HSteamNetConnection_Invalid)
        return false;

    steamNetworkingSockets->
        CloseConnection(steamNetworkConnection, 0, "Disconnected", true);

    mapConnections.erase(steamNetworkConnection);
    return true;
}

void TransportGNS::poll()
{
    pollConnectionStateChanges();
    pollIncomingMessages();
}

void TransportGNS::pollConnectionStateChanges() const
{
    steamNetworkingSockets->RunCallbacks();
}

void TransportGNS::pollIncomingMessages()
{
    if (pollGroup != k_HSteamNetPollGroup_Invalid)
    {
        ISteamNetworkingMessage* steamMessage = nullptr;
        int numberOfmessages = steamNetworkingSockets->ReceiveMessagesOnConnection(pollGroup, &steamMessage, 1);

        if (numberOfmessages < 1) return;

        int connectionId = getConnectionId(steamMessage->m_conn);

        if (onMessageReceived)
        {
            IncomingRawMessage incoming(
                connectionId,
                static_cast<const std::byte*>(steamMessage->m_pData),
                steamMessage->m_cbSize
            );
            onMessageReceived(incoming);
        }

        steamMessage->Release();
    }

    for (auto& [steamNetworkConnection, connectionId] : mapConnections)
    {
        ISteamNetworkingMessage* steamMessage = nullptr;
        while
        (
            steamNetworkingSockets->
            ReceiveMessagesOnConnection(steamNetworkConnection, &steamMessage, 1) == 1
        )
        {
            IncomingRawMessage incoming(
                connectionId,
                static_cast<const std::byte*>(steamMessage->m_pData),
                steamMessage->m_cbSize
            );

            if (onMessageReceived)
            {
                onMessageReceived(incoming);
            }

            steamMessage->Release();
        }
    }
}

void TransportGNS::steamNetConnectionStatusChangedCallback(
    SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    transportGNSCallbackInstance->onSteamNetConnectionStatusChanged(pointerConnectionStatusInformation);
}

void TransportGNS::onSteamNetConnectionStatusChanged(const SteamNetConnectionStatusChangedCallback_t*
    pointerConnectionStatusInformation)
{
    switch (pointerConnectionStatusInformation->m_info.m_eState)
    {
    case k_HSteamNetConnection_Invalid:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Error;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            onConnectionChanged(connection);
            break;
        }
    case k_ESteamNetworkingConnectionState_Dead:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Death;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            onConnectionChanged(connection);
            break;
        }
    case k_ESteamNetworkingConnectionState_Connecting:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Connecting;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            onConnectionChanged(connection);
            break;
        }

    case k_ESteamNetworkingConnectionState_Connected:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Connected;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            break;
        }
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Error;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            onConnectionChanged(connection);
            break;
        }
    default:
        Connection connection{};
        connection.connectionStatus = ConnectionStatus::Invalid;
        connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
        onConnectionChanged(connection);
        break;
    }
}

int TransportGNS::getConnectionId(HSteamNetConnection steamNetworkConnection)
{
    auto connectionCombo = mapConnections.find(steamNetworkConnection);
    if (connectionCombo != mapConnections.end())
        return connectionCombo->second;
    return -1;
}

HSteamNetConnection TransportGNS::getSteamConnection(int connectionId)
{
    for (const auto& [steamConnection, mappedConnectionId] : mapConnections)
    {
        if (mappedConnectionId == connectionId)
            return steamConnection;
    }
    return k_HSteamNetConnection_Invalid;
}

std::vector<int> TransportGNS::getActiveConnectionIds() const
{
    std::vector<int> ids;
    ids.reserve(mapConnections.size());

    for (auto& connectionPair : mapConnections)
    {
        ids.push_back(connectionPair.second);
    }
    return ids;
}

int TransportGNS::getSendFlags(SendMode sendMode)
{
    switch (sendMode)
    {
    case SendMode::ReliableOrdered:
        return k_nSteamNetworkingSend_Reliable;
    case SendMode::ReliableUnordered:
        return k_nSteamNetworkingSend_NoDelay;
    case SendMode::Unreliable:
        return k_nSteamNetworkingSend_Unreliable;
    default:
        return k_nSteamNetworkingSend_Reliable;
    }
}
