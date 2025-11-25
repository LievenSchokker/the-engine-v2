#include "Networking/Server/Server.h"
#include "Networking/TransportGNS.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/Concretes/ConnectionMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include "Networking/TransportResult.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include "Networking/MessageHandlers/IMessageHandler.h"


#include <iostream>


Server::Server(const ServerConnectionInformation& serverConnectionInformation,
               std::unique_ptr<ITransport> injectedTransport)
    : transport(std::move(injectedTransport))
    , status(ServerStatus::Stopping)
{
    if (serverConnectionInformation.port == 0)
    {
        throw std::runtime_error("Port is not set");
    }
    setupInformation = serverConnectionInformation;

    messageDispatcher = spelmotor_networking::MessageDispatcherFactory::createMessageDispatcher();
}

Server::~Server()
{
    stop();
}

ServerStatus Server::start()
{
    transport->setOnMessageReceived([this](const IncomingRawMessage& message)
    {
        onMessage(message);
    });

    transport->setOnConnectionChanged([this](const Connection& connection)
    {
        onConnectionChanged(connection);
    });

    TransportResult result = transport->setUpListenSocket(setupInformation.port);

    if (result == TransportResult::SUCCESS)
    {
        status = ServerStatus::Running;
        std::cout << "Started successfully on port " << setupInformation.port << std::endl;
    }
    else
    {
        status = ServerStatus::Error;
        std::cerr << "Failed to start" << std::endl;
    }

    return status;
}

void Server::update() const
{
    transport->poll();
}

ServerStatus Server::stop()
{
    if (status == ServerStatus::Running)
    {
        transport->closeOpenSocket();
        connectedClients.clear();
        status = ServerStatus::Stopping;
        std::cout << "Stopped" << std::endl;
    }
    return status;
}

void Server::onConnectionChanged(const Connection& connection)
{
    const int clientId = connection.transportConnectionId;

    switch (connection.connectionStatus)
    {
    case ConnectionStatus::Connected:
        connectedClients.insert(clientId);
        std::cout << "Client " << clientId << " connected" << std::endl;
        break;

    case ConnectionStatus::Terminated:
    	break;
    case ConnectionStatus::Error:
        connectedClients.erase(clientId);
        std::cout << "Client " << clientId << " disconnected" << std::endl;
        break;

    default:
        break;
    }
}

void Server::onMessage(const IncomingRawMessage& rawMessage)
{
    // Ignore messages from unknown connections
    if (!connectedClients.contains(rawMessage.connectionID))
    {
        std::cerr << "Message from unknown client " << rawMessage.connectionID << std::endl;
        return;
    }

    const std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (!message)
    {
        std::cerr << "Failed to parse message from client " << rawMessage.connectionID << std::endl;
        return;
    }

    MessageTypes messageType = message->getMessageType();
    const int clientId = rawMessage.connectionID;

    switch (messageType)
    {
    case MessageTypes::ConnectionMessage:
        if (auto* connMsg = dynamic_cast<ConnectionMessage*>(message.get()))
        {
            handleConnectionMessage(clientId, connMsg);
        }
        break;

    default:
        std::cerr << "Unknown message type: " << static_cast<int>(messageType) << std::endl;
        break;
    }
}

void Server::handleConnectionMessage(int clientId, ConnectionMessage* message)
{
    switch (message->getStatus())
    {
    case ConnectionStatus::Disconnected:
        transport->disconnectFromSocket(clientId);
        connectedClients.erase(clientId);
        break;

    default:
        break;
    }
}

bool Server::sendMessage(const int clientId, const IMessage& message, const SendMode& mode) const
{
    if (!connectedClients.contains(clientId))
    {
        return false;
    }

    const OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        message,
        clientId,
        mode
    );

    return transport->send(outgoing) == TransportResult::SUCCESS;
}

bool Server::sendMessage(const int clientId, const IMessage& message) const
{
    return sendMessage(clientId, message, SendMode::ReliableOrdered);
}

bool Server::broadcastMessage(const IMessage& message) const
{
    bool allSucceeded = true;

    for (const int clientId : connectedClients)
    {
        if (!sendMessage(clientId, message))
        {
            allSucceeded = false;
        }
    }

    return allSucceeded;
}

bool Server::broadcastMessage(const IMessage& message, const int excludeClientId) const
{
    bool allSucceeded = true;

    for (const int clientId : connectedClients)
    {
        if (clientId != excludeClientId)
        {
            if (!sendMessage(clientId, message))
            {
                allSucceeded = false;
            }
        }
    }

    return allSucceeded;
}

void Server::kickClient(const int clientId)
{
    ConnectionMessage disconnectMessage;
    disconnectMessage.setStatus(ConnectionStatus::Disconnected);

    const OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        disconnectMessage,
        clientId,
        SendMode::ReliableOrdered
    );

    if (transport->send(outgoing) == TransportResult::SUCCESS)
    {
        transport->disconnectFromSocket(clientId);
        connectedClients.erase(clientId);
    }
}
