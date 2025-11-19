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

Client::Client()
    : connectionManager(std::make_unique<ConnectionManager>(ConnectionMode::Client))
{
    setDefaultOnMessageReceived();
    setDefaultOnConnectionChanged();
}

Client::~Client()
{
    running = false;
    connectionManager->shutdown();
    if (listenThread.joinable())
        listenThread.join();
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
        }
    });

    ConnectionStatus status = connectionManager->init(serverInfo, ConnectionMode::Client);

    if (status != ConnectionStatus::Connected)
    {
        std::cerr << "Failed to connect to " << serverIP << ":" << port << "\n";
        return false;
    }

    std::cout << "Connecting to " << serverIP << ":" << port << "\n";

    running = true;
    createListenThread();

    return true;
}

bool Client::sendMessage(IMessage& message)
{
    if (!connected)
    {
        std::cerr << "[Client] Cannot send message - not connected\n";
        return false;
    }

    if (clientConnectionId == -1)
    {
        std::cerr << "[Client] Cannot send message - invalid connection ID\n";
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
}

void Client::setDefaultOnMessageReceived()
{
    connectionManager->setOnMessageCallback([this](const IncomingRawMessage& rawMessage)
    {
        std::unique_ptr<IMessage> message = MessageReader::readMessage(rawMessage);

        if (!message)
        {
            std::cerr << "[Client] Failed to parse message from server\n";
            return;
        }

        MessageTypes messageType = message->getMessageType();

        std::cout << "[Client] Received message type: "
            << static_cast<int>(messageType) << "\n";

        switch (messageType)
        {
        case MessageTypes::ConnectionMessage:
            {
                ConnectionMessage* connMsg = static_cast<ConnectionMessage*>(message.get());
                ConnectionStatus status = connMsg->getStatus();

                if (status == ConnectionStatus::Connected)
                {
                    connected = true;
                    clientConnectionId = rawMessage.connectionID;
                }
                else if (status == ConnectionStatus::Disconnected)
                {
                    connected = false;
                    running = false;
                    std::cout << "[Client] Disconnected by server\n";
                }
                break;
            }
        default:
            {
                std::cout << "Unhandled message type: "
                    << static_cast<int>(messageType) << "\n";
                break;
            }
        }
    });
}


void Client::setDefaultOnConnectionChanged()
{
}

void Client::createListenThread()
{
    listenThread = std::thread([this]()
    {
        std::cout << "[Client] Listen thread started\n";

        while (running)
        {
            connectionManager->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::cout << "[Client] Listen thread stopped\n";
    });
}
