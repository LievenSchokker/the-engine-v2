#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/SendMode.h"
#include "Networking/TransportResult.h"


#include <iostream>


Client::Client()
    : transport(std::make_unique<TransportGNS>())
{
    currentConnection.connectionStatus = ConnectionStatus::Disconnected;

    transport->setOnMessageReceived([this](const IncomingRawMessage& message)
    {
        onMessageReceived(message);
    });

    transport->setOnConnectionChanged([this](const Connection& connection)
    {
        onConnectionChanged(connection);
    });
}

Client::~Client()
{
    disconnect();
}

bool Client::connectToServer(const uint16_t port, const char* serverIP) const
{
    if (transport->connectByIPAdress(serverIP, port) != TransportResult::SUCCESS)
    {
        std::cerr << "Failed to connect to " << serverIP << ":" << port << std::endl;
        return false;
    }

    return true;
}

void Client::disconnect()
{
    if (currentConnection.connectionStatus == ConnectionStatus::Connected)
    {
        transport->disconnectFromSocket(currentConnection.transportConnectionId);
    }
    transport->closeOpenSocket();
    currentConnection.connectionStatus = ConnectionStatus::Disconnected;
}

bool Client::sendMessage(const IMessage& message) const
{
    if (currentConnection.connectionStatus != ConnectionStatus::Connected)
    {
        return false;
    }

    OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        message,
        currentConnection.transportConnectionId,
        SendMode::ReliableOrdered
    );

    TransportResult result = transport->send(outgoing);
    return result == TransportResult::SUCCESS;
}

void Client::poll() const
{
    transport->poll();
}

bool Client::isConnected() const
{
    return currentConnection.connectionStatus == ConnectionStatus::Connected;
}

void Client::onConnectionChanged(const Connection& connection)
{
    currentConnection = connection;

    switch (connection.connectionStatus)
    {
    case ConnectionStatus::Connected:
        std::cout << "Connected to server" << std::endl;
        break;

    case ConnectionStatus::Connecting:
        std::cout << "Connecting..." << std::endl;
        break;

    case ConnectionStatus::Error:
        disconnect();
        std::cout << "Disconnected from server" << std::endl;
        break;

    default:
        break;
    }
}

void Client::onMessageReceived(const IncomingRawMessage& rawMessage)
{
    const std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (!message)
    {
        std::cerr << "Failed to parse message" << std::endl;
        return;
    }

    
    switch (MessageTypes messageType = message->getMessageType())
    {
    case MessageTypes::ConnectionMessage:
        {
            if (dynamic_cast<ConnectionMessage*>(message.get())->getStatus() == ConnectionStatus::Disconnected)
            {
                disconnect();
            }
            break;
        }
    default:
        break;
    }
}
