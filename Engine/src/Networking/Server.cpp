#include "Networking/Server/Server.h"
#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/Messages/IncommingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include <iostream>

Server::Server(const ServerConnectionInformation& serverConnectionInformation)
    : connectionManager(std::make_unique<ConnectionManager>(ConnectionMode::Server))
      , status(ServerStatus::Stopping)
{
    if (serverConnectionInformation.port == 0)
    {
        throw std::runtime_error("Port is not set");
    }

    if (serverConnectionInformation.ip.empty())
    {
        throw std::runtime_error("IP is not set");
    }

    setupInformation = serverConnectionInformation;
}

Server::~Server()
{
    stop();
}

ServerStatus Server::start()
{
    // Initialize connection manager
    ConnectionStatus connectionStatus = connectionManager->init(setupInformation, ConnectionMode::Server);

    // Set up message callback
    connectionManager->setOnMessageCallback([this](IncomingRawMessage message)
    {
        onMessage(message);
    });

    if (connectionStatus == ConnectionStatus::Connected)
    {
        status = ServerStatus::Running;
        std::cout << "[Server] Started successfully on port "
            << setupInformation.port << "\n";
    }
    else
    {
        status = ServerStatus::Error;
        std::cerr << "[Server] Failed to start\n";
    }

    return status;
}

void Server::update()
{
    // Poll for new messages and connection changes
    connectionManager->poll();
}

ServerStatus Server::stop()
{
    if (status == ServerStatus::Running)
    {
        connectionManager->shutdown();
        connectedClients.clear();
        status = ServerStatus::Stopping;
        std::cout << "[Server] Stopped\n";
    }
    return status;
}

void Server::onMessage(IncomingRawMessage rawMessage)
{
    std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (!message)
    {
        std::cerr << "Failed to parse message from client "
            << rawMessage.connectionID << "\n";
        return;
    }

    MessageTypes messageType = message->getMessageType();
    int clientId = rawMessage.connectionID;

    std::cout << "Received message type "
        << static_cast<int>(messageType)
        << " from client " << clientId << "\n";

    switch (messageType)
    {
    case MessageTypes::ConnectionMessage:
        {
            handleConnectionMessage(clientId, static_cast<ConnectionMessage*>(message.get()));
            break;
        }
    default:
        {
            throw std::runtime_error("Unknown message type");
        }
    }
}

void Server::handleConnectionMessage(int clientId, ConnectionMessage* message)
{
    ConnectionStatus status = message->getStatus();

    std::cout << "Client " << clientId
        << " connection message with status: "
        << static_cast<int>(status) << "\n";

    switch (status)
    {
    case ConnectionStatus::Connected:
        {
            connectedClients.insert(clientId);

            ConnectionMessage response;
            response.setStatus(ConnectionStatus::Connected);

            OutgoingRawMessage outgoing = MessageWriter::writeMessage(
                response,
                clientId,
                SendMode::ReliableOrdered
            );

            connectionManager->send(outgoing);
            break;
        }

    case ConnectionStatus::Disconnected:
        {
            connectionManager->disconnect(clientId);
            connectedClients.erase(clientId);
            break;
        }

    case ConnectionStatus::Error:
        {
            break;
        }
    }
}


void Server::kickClient(int clientId)
{
    ConnectionMessage disconnectMsg;
    disconnectMsg.setStatus(ConnectionStatus::Disconnected);

    OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        disconnectMsg,
        clientId,
        SendMode::ReliableOrdered
    );

    connectionManager->send(outgoing);

    connectionManager->disconnect(clientId);

    connectedClients.erase(clientId);
}
