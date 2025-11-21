// Client.cpp
#include "Networking/Client.h"
#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Messages/IMessage.h"
#include "Networking/Messages/MessageReader.h"
#include "Networking/Messages/MessageWriter.h"
#include "Networking/Messages/ConnectionMessage.h"
#include "Networking/Messages/IncommingRawMessage.h"
#include "Networking/Messages/OutgoingRawMessage.h"
#include "Networking/Messages/MessageTypes.h"
#include "Networking/SendMode.h"

#include <iostream>
#include <thread>
#include <chrono>

Client::Client()
    : connectionManager(std::make_unique<ConnectionManager>(ConnectionMode::Client))
{
    connectionManager->setOnMessageCallback([this](const IncomingRawMessage& rawMessage)
    {
        onMessageReceived(rawMessage);
    });
}

Client::~Client()
{
    if (listenThread.joinable())
        listenThread.join();
    connectionManager->shutdown();
}

bool Client::connectToServer(const uint16_t port, const char* serverIP)
{
    ServerConnectionInformation serverInfo;
    serverInfo.ip = serverIP;
    serverInfo.port = port;

    connectionManager->setOnConnectionChangedCallback([this](int connId, bool isConnected)
    {
        if (isConnected)
        {
            clientConnectionId = connId;
            connected = true;
        }
        else
        {
            connected = false;
            clientConnectionId = -1;
            running = false;
        }
    });

    ConnectionStatus status = connectionManager->init(serverInfo, ConnectionMode::Client);

    if (status != ConnectionStatus::Connected)
    {
        return false;
    }
    createListenThread();

    return true;
}

bool Client::sendMessage(IMessage& message)
{
    if (currentConnection.connectionStatus == ConnectionStatus::Disconnected)
    {
        return false;
    }

    OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        message,
        currentConnection.transportConnectionId,
        SendMode::ReliableOrdered
    );

    connectionManager->send(outgoing);

    return true;
}

void Client::onMessageReceived(const IncomingRawMessage& rawMessage)
{
    std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

    if (!message)
    {
        return;
    }

    MessageTypes messageType = message->getMessageType();

    switch (messageType)
    {
    case MessageTypes::ConnectionMessage:
        {
            ConnectionMessage* connMsg = static_cast<ConnectionMessage*>(message.get());
            ConnectionStatus status = connMsg->getStatus();
            break;
        }
    default:
        {
            break;
        }
    }
}

void Client::onConnectionChanged()
{
    // Already set in connectToServer
}

void Client::poll()
{
    connectionManager->poll();
}
