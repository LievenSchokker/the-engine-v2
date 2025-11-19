// ConnectionManager.cpp


#include <iostream>


#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Messages/IncommingRawMessage.h"


ConnectionManager::ConnectionManager(ConnectionMode mode)
    : transport(std::make_unique<TransportGNS>())
      , maxConnections(100)
      , mode(mode)
{
}

ConnectionManager::~ConnectionManager() = default;

ConnectionStatus ConnectionManager::init(const ServerConnectionInformation& information,
                                         ConnectionMode connectionMode)
{
    mode = connectionMode;

    if (!transport)
    {
        return ConnectionStatus::Error;
    }

    transport->setOnMessageReceived([this](const IncomingRawMessage& msg)
    {
        handleTransportMessage(msg);
    });

    transport->setOnConnectionChanged([this](int connId, bool connected) {
        if (connected)
        {
            Connection conn = {connId, ConnectionStatus::Connected};
            connections[connId] = conn;

            if (mode == ConnectionMode::Server)
            {
                std::cout << "[ConnectionManager] Client " << connId << " connected\n";
            }
        }
        else
        {
            connections.erase(connId);

            if (mode == ConnectionMode::Server)
            {
                std::cout << "[ConnectionManager] Client " << connId << " disconnected\n";
            }
        }

        if (onConnectionChanged)
        {
            onConnectionChanged(connId, connected);
        }
    });

    TransportResult result;
    if (mode == ConnectionMode::Server)
    {
        result = transport->setUpListenSocket(information.port);
    }
    else
    {
        result = transport->connectByIPAdress(information.ip.c_str(), information.port);
    }

    return (result == TransportResult::SUCCES) ? ConnectionStatus::Connected : ConnectionStatus::Error;
}

TransportResult ConnectionManager::send(const OutgoingRawMessage& message) const
{
    return transport->send(message);
}

void ConnectionManager::disconnect(int networkId) const
{
    transport->disconnectFromSocket(networkId);
}

void ConnectionManager::shutdown()
{
    if (transport)
    {
        transport->closeOpenSocket();
    }
    connections.clear();
}

void ConnectionManager::handleTransportMessage(const IncomingRawMessage& message)
{
    if (connections.find(message.connectionID) != connections.end())
    {
        if (onMessage)
        {
            onMessage(message);
        }
    }
    else
    {
        std::cerr << "[ConnectionManager] Received message from unknown connection: "
            << message.connectionID << "\n";
    }
}

void ConnectionManager::setOnMessageCallback(std::function<void(IncomingRawMessage)> callback)
{
    onMessage = std::move(callback);
}

void ConnectionManager::setOnConnectionChangedCallback(std::function<void(int, bool)> callback)
{
    onConnectionChanged = std::move(callback);
}

void ConnectionManager::poll() const
{
    if (transport)
    {
        transport->poll();
    }
}
