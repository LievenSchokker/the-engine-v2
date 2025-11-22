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
    transportGNSCallbackInstance = this;

    SteamDatagramErrMsg errorMessage;

    if (!GameNetworkingSockets_Init(nullptr, errorMessage))
    {
        return;
    }

    steamNetworkingSockets = SteamNetworkingSockets();
    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, debugOutput);
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

    return TransportResult::SUCCESS;
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

    return TransportResult::SUCCESS;
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

    return (result == k_EResultOK) ? TransportResult::SUCCESS : TransportResult::ERROR;
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

    return TransportResult::SUCCESS;
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
    constexpr int maxMessages = 64;
    ISteamNetworkingMessage* messages[maxMessages];

    // Server mode: use poll group
    if (pollGroup != k_HSteamNetPollGroup_Invalid)
    {
        int messageCount = steamNetworkingSockets->ReceiveMessagesOnPollGroup(
            pollGroup, messages, maxMessages
        );

        for (int i = 0; i < messageCount; ++i)
        {
            processMessage(messages[i]);
            messages[i]->Release();
        }
        return;
    }

    // Client mode: poll individual connections
    for (const auto& [steamConnection, connectionId] : mapConnections)
    {
        int messageCount = steamNetworkingSockets->ReceiveMessagesOnConnection(
            steamConnection, messages, maxMessages
        );

        for (int i = 0; i < messageCount; ++i)
        {
            processMessage(messages[i]);
            messages[i]->Release();
        }
    }
}

void TransportGNS::processMessage(ISteamNetworkingMessage* steamMessage)
{
    if (!steamMessage || !onMessageReceived)
        return;

    int connectionId = getConnectionId(steamMessage->m_conn);

    IncomingRawMessage incoming(
        connectionId,
        static_cast<const std::byte*>(steamMessage->m_pData),
        steamMessage->m_cbSize
    );

    onMessageReceived(incoming);
}

void TransportGNS::steamNetConnectionStatusChangedCallback(
    SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    if (transportGNSCallbackInstance)
    {
        transportGNSCallbackInstance->onSteamNetConnectionStatusChanged(pointerConnectionStatusInformation);
    }
}

void TransportGNS::onSteamNetConnectionStatusChanged(const SteamNetConnectionStatusChangedCallback_t*
    pointerConnectionStatusInformation)
{
    switch (pointerConnectionStatusInformation->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_None:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Error;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            safeOnConnectionChanged(connection);
            break;
        }
    case k_ESteamNetworkingConnectionState_Dead:
        {
            removeDeathConnection(pointerConnectionStatusInformation);
            break;
        }
    case k_ESteamNetworkingConnectionState_Connecting:
        {
            if (listenSocket != k_HSteamListenSocket_Invalid)
            {
                steamNetworkingSockets->AcceptConnection(pointerConnectionStatusInformation->m_hConn);
                steamNetworkingSockets->SetConnectionPollGroup(pointerConnectionStatusInformation->m_hConn, pollGroup);
            }

            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Connecting;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            safeOnConnectionChanged(connection);
            break;
        }

    case k_ESteamNetworkingConnectionState_Connected:
        {
            addNewConnection(pointerConnectionStatusInformation);
            break;
        }
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            Connection connection{};
            connection.connectionStatus = ConnectionStatus::Error;
            connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
            mapConnections.erase(pointerConnectionStatusInformation->m_hConn);
            steamNetworkingSockets->CloseConnection(pointerConnectionStatusInformation->m_hConn, 0, nullptr, false);
            safeOnConnectionChanged(connection);
            break;
        }
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
        {
            removeDeathConnection(pointerConnectionStatusInformation);
            break;
        }
    default:
        Connection connection{};
        connection.connectionStatus = ConnectionStatus::Invalid;
        connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
        safeOnConnectionChanged(connection);
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

void TransportGNS::safeOnConnectionChanged(const Connection& connection)
{
    if (onConnectionChanged)
    {
        onConnectionChanged(connection);
    }
}

void TransportGNS::addNewConnection(const SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    HSteamNetConnection steamConn = pointerConnectionStatusInformation->m_hConn;

    int connectionId = nextConnectionId++;
    mapConnections[steamConn] = connectionId;
    Connection connection{};
    connection.connectionStatus = ConnectionStatus::Connected;
    connection.transportConnectionId = connectionId;

    //After creating the new connection still need to notify other layers.
    safeOnConnectionChanged(connection);
}

void TransportGNS::removeDeathConnection(
    const SteamNetConnectionStatusChangedCallback_t* pointerConnectionStatusInformation)
{
    Connection connection{};
    connection.connectionStatus = ConnectionStatus::Death;
    connection.transportConnectionId = getConnectionId(pointerConnectionStatusInformation->m_hConn);
    mapConnections.erase(pointerConnectionStatusInformation->m_hConn);
    safeOnConnectionChanged(connection);
}


void TransportGNS::debugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* message)
{
    std::cout << message << std::endl;
}
