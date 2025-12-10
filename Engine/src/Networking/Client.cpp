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
<<<<<<< HEAD
=======
#include "Networking/Messages/MessageDispatcherFactory.h"
>>>>>>> origin/development

#include <iostream>


Client::Client(std::unique_ptr<ITransport> injectedTransport)
<<<<<<< HEAD
	: transport(std::move(injectedTransport))
=======
    : transport(std::move(injectedTransport)),
    gameWorld(nullptr)
>>>>>>> origin/development
{
	currentConnection.connectionStatus = ConnectionStatus::Disconnected;

	transport->setOnMessageReceived([this](const IncomingRawMessage& message)
	{
		onMessageReceived(message);
	});

<<<<<<< HEAD
	transport->setOnConnectionChanged([this](const Connection& connection)
	{
		onConnectionChanged(connection);
	});
=======
    transport->setOnConnectionChanged([this](const Connection& connection)
    {
        onConnectionChanged(connection);
    });

    messageDispatcher = spelmotor_networking::MessageDispatcherFactory::createMessageDispatcher(ConnectionMode::Client, *gameWorld);
>>>>>>> origin/development
}


Client::~Client()
{
	disconnect();
}

<<<<<<< HEAD
bool Client::connectToServer(
	const ServerConnectionInformation& serverInformartion) const
=======

bool Client::connectToServer(const ServerConnectionInformation&  serverInformartion) const
>>>>>>> origin/development
{
	if (transport->connectByIPAdress(serverInformartion.ip.c_str(),
	                                 serverInformartion.port) !=
	    TransportResult::SUCCESS)
	{
		std::cerr << "Failed to connect to " << serverInformartion.ip << ":" <<
			serverInformartion.port << std::endl;
		return false;
	}
	return true;
}


void Client::disconnect()
{
	if (currentConnection.connectionStatus == ConnectionStatus::Connected)
	{
		transport->
			disconnectFromSocket(currentConnection.transportConnectionId);
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
			break;

		case ConnectionStatus::Connecting:
			break;

		case ConnectionStatus::Error:
			disconnect();
			break;

		default:
			break;
	}
}

<<<<<<< HEAD
void Client::onMessageReceived(const IncomingRawMessage& rawMessage) const
{
	const std::unique_ptr<IMessage> message = MessageReader::readMessage(
		rawMessage);

	if (!message)
	{
		std::cerr << "Failed to parse message" << std::endl;
		return;
	}

	messageDispatcher->processMessage(*message);
}

void Client::injectMessageDispatcher(
	std::unique_ptr<spelmotor_networking::MessageDispatcher> dispatcher)
=======

void Client::onMessageReceived(const IncomingRawMessage& rawMessage) const
{
    std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (message == nullptr)
    {
        std::cerr << "Failed to parse message" << std::endl;
        return;
    }

    messageDispatcher->processMessage(std::move(message));
}


void Client::injectMessageDispatcher(std::unique_ptr<spelmotor_networking::MessageDispatcher> dispatcher)
>>>>>>> origin/development
{
	messageDispatcher = std::move(dispatcher);
}