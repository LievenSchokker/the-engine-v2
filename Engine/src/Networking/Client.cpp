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
    setDefaultOnMessageReceived();
    setDefaultOnConnectionChanged();
}

Client::~Client()
{
    running = false;
    if (listenThread.joinable())
        listenThread.join();
    connectionManager->shutdown();
}

bool Client::connectToServer(const uint16_t port, const char* serverIP)
{
    ServerConnectionInformation serverInfo;
    serverInfo.ip = serverIP;
    serverInfo.port = port;

    connectionManager->setOnConnectionChangedCallback([this](int connId, bool isConnected) {
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
    running = true;
    createListenThread();

    return true;
}

bool Client::sendMessage(IMessage& message)
{
    if (!connected)
    {
        return false;
    }

    if (clientConnectionId == -1)
    {
        return false;
    }

    OutgoingRawMessage outgoing = MessageWriter::writeMessage(
        message,
        clientConnectionId,
        SendMode::ReliableOrdered
    );

    connectionManager->send(outgoing);

    return true;
}

void Client::setOnMessageReceived(const OnMessageReceivedCallback& callback)
{
    connectionManager->setOnMessageCallback(callback);
}

void Client::setOnConnectionChanged(const OnConnectionChangedCallback& callback)
{
    // Could store and forward to user callback if needed
}

void Client::setDefaultOnMessageReceived()
{
    connectionManager->setOnMessageCallback([this](const IncomingRawMessage& rawMessage)
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

                if (status == ConnectionStatus::Disconnected)
                {
                    connected = false;
                    running = false;
                }
                break;
            }
        default:
            {
                break;
            }
        }
    });
}

void Client::setDefaultOnConnectionChanged()
{
    // Already set in connectToServer
}

void Client::createListenThread()
{
    listenThread = std::thread([this]()
    {
        while (running)
        {
            connectionManager->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}