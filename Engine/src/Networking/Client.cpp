#include "Networking/Client.h"
#include "Networking/TransportGNS.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include "Networking/TransportResult.h"
#include "Networking/Messages/MessageDispatcherFactory.h"
#include <iostream>


Client::Client(std::unique_ptr<ITransport> injectedTransport)
    : transport(std::move(injectedTransport))
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
    messageDispatcher = nullptr;
}


Client::~Client()
{
    disconnect();
}


bool Client::connectToServer(const ServerConnectionInformation&  serverInformartion) const
{
    if (transport->connectByIPAdress(serverInformartion.ip.c_str(), serverInformartion.port) != TransportResult::SUCCESS)
    {
        std::cerr << "Failed to connect to " << serverInformartion.ip << ":" << serverInformartion.port << std::endl;
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

    const OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        message,
        currentConnection.transportConnectionId,
        SendMode::ReliableOrdered
    );

    const TransportResult result = transport->send(outgoing);
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


void Client::onMessageReceived(const IncomingRawMessage& rawMessage) const
{
    std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

	if (message->getMessageType() == MessageTypes::SpawnMessage)
	{
		std::cout << rawMessage.length << std::endl;
	}

    if (message == nullptr)
    {
        std::cerr << "Failed to parse message" << std::endl;
        return;
    }

    messageDispatcher->processMessage(std::move(message));
}


void Client::injectMessageDispatcher(std::unique_ptr<spelmotor_networking::MessageDispatcher> dispatcher)
{
    messageDispatcher = std::move(dispatcher);
}
