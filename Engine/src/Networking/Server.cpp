#include "Networking/Server/Server.h"

#include "Core/ApplicationSpecifications.h"
#include "Networking/Connection/Connection.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Messages/ConcreteMessages/ConnectionMessage.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/IncomingRawMessage.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/ConcreteMessages/MessageTypes.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/SendMode.h"
#include "Networking/TransportGNS.h"
#include "Networking/TransportResult.h"

#include <iostream>

#include "Networking/Messages/ConcreteMessages/WelcomeMessage.h"

Server::Server(const ServerConnectionInformation& serverConnectionInformation,
               std::unique_ptr<ITransport> injectedTransport)
	: transport(std::move(injectedTransport))
	  , status(ServerStatus::Stopping),
	  messageDispatcher(nullptr)
	  , spawnManager(nullptr)
{
	if (serverConnectionInformation.port == 0)
	{
		throw std::runtime_error("Port is not set");
	}
	setupInformation = serverConnectionInformation;
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

	TransportResult result = transport->
		setUpListenSocket(setupInformation.port);

	if (result == TransportResult::SUCCESS)
	{
		status = ServerStatus::Running;
		std::cout << "Started successfully on port " << setupInformation.port <<
			std::endl;
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
	    {
	        connectedClients.insert(clientId);
	        handleNewClientConnected(clientId);
	        if (onClientConnected)
	        {
	            onClientConnected(clientId);
	        }
	        WelcomeMessage msg(clientId);
	        sendMessage(clientId, msg, SendMode::ReliableOrdered);
	        break;
	    }
		case ConnectionStatus::Terminated:
			break;
		case ConnectionStatus::Error:
			connectedClients.erase(clientId);
			if (onClientDisconnected)
			{
				onClientDisconnected(clientId);
			}
			break;

		default:
			break;
	}
}


void Server::onMessage(const IncomingRawMessage& rawMessage)
{
	if (!connectedClients.contains(rawMessage.connectionID))
	{
		std::cerr << "Message from unknown client " << rawMessage.connectionID
			<< std::endl;
		return;
	}

	std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

	if (!message)
	{
		std::cerr << "Failed to parse message from client " << rawMessage.
			connectionID << std::endl;
		return;
	}

	MessageTypes messageType = message->getMessageType();
	const int clientId = rawMessage.connectionID;

	if (messageType == MessageTypes::ConnectionMessage)
	{
		if (auto* connMsg = dynamic_cast<ConnectionMessage*>(message.get()))
		{
			handleConnectionMessage(clientId, connMsg);
		}
		return;
	}

	if (messageDispatcher)
	{
		messageDispatcher->processMessage(std::move(message));
	}
	else
	{
		std::cerr << "No message dispatcher set!" << std::endl;
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

void Server::handleNewClientConnected(int clientId) const
{
    auto message = std::make_unique<WelcomeMessage>(clientId);
    messageDispatcher->processMessage(std::move(message));
}

bool Server::sendMessage(const int clientId, const IMessage& message,
                         const SendMode& mode) const
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
	return sendMessage(clientId, message, SendMode::Unreliable);
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


bool Server::broadcastMessage(const IMessage& message,
                              const int excludeClientId) const
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


void Server::injectMessageDispatcher(
	std::unique_ptr<spelmotorNetworking::MessageDispatcher> dispatcher)
{
	messageDispatcher = std::move(dispatcher);
}

void Server::setClientConnectedCallback(ClientConnectedCallback callback)
{
	onClientConnected = std::move(callback);
}

void Server::setClientDisconnectedCallback(ClientDisconnectedCallback callback)
{
	onClientDisconnected = std::move(callback);
}

ServerConnectionInformation Server::convertApplicationSettings(
	const ApplicationSpecifications& specifications)
{
	ServerConnectionInformation server;
	server.ip = specifications.networkingOptions.serverIP;
	server.port = specifications.networkingOptions.port;
	return server;
}