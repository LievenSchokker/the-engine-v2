#include "Networking/Server/Server.h"
#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include <iostream>

#include "Networking/TransportResult.h"

Server::Server(const ServerConnectionInformation& serverConnectionInformation)
    : connectionManager(std::make_unique<ConnectionManager>(ConnectionMode::Host))
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
    ConnectionStatus connectionStatus = connectionManager->init(setupInformation, ConnectionMode::Host);

    connectionManager->setOnMessageCallback([this](IncomingRawMessage message)
    {
        onMessage(message);
    });

    if (connectionStatus == ConnectionStatus::Connected)
    {
        status = ServerStatus::Running;
        std::cout << "Started successfully on port "
            << setupInformation.port << std::endl;
    }
    else
    {
        status = ServerStatus::Error;
        std::cerr << "Failed to start" << std::endl;
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
        std::cout << "Stopped" << std::endl;
    }
    return status;
}

void Server::onMessage(IncomingRawMessage rawMessage)
{
    std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (!message)
    {
        std::cerr << "Failed to parse message from client "
            << rawMessage.connectionID << std::endl;
        return;
    }

    MessageTypes messageType = message->getMessageType();
    int clientId = rawMessage.connectionID;

    std::cout << "Received message type "
        << static_cast<int>(messageType)
        << " from client " << clientId << std::endl;

    switch (messageType)
    {
    case MessageTypes::ConnectionMessage:
        {
            if (auto* connMsg = dynamic_cast<ConnectionMessage*>(message.get())) {
                handleConnectionMessage(clientId, connMsg);
            } else {
                std::cerr << "Message type mismatch" << std::endl;
            }
            break;
        }
    default:
        {
            std::cerr << "Unknown message type: " << static_cast<int>(messageType) << std::endl;
            break;
        }
    }
}

void Server::handleConnectionMessage(int clientId, ConnectionMessage* message)
{
    ConnectionStatus status = message->getStatus();

    std::cout << "Client " << clientId
        << " connection message with status: "
        << static_cast<int>(status) << std::endl;

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

    TransportResult  result = connectionManager->send(outgoing);

    if (result == TransportResult::SUCCES)
    {
        connectionManager->disconnect(clientId);
        connectedClients.erase(clientId);
    }
}
