#include "core/Networking/Server/Server.h"

#include <iostream>

#include "core/Networking/Connection/ConnectionMode.h"
#include "core/Networking/Connection/ConnectionStatus.h"

Server::Server(const ServerConnectionInformation & serverConnectionInformation)
    : connectionManager(std::make_unique<ConnectionManager>()),
    status(ServerStatus::Stopping)
{
    if (serverConnectionInformation.port == 0)
    {
        throw std::runtime_error("Port is not set");
    }

    if (serverConnectionInformation.ip.empty() == true)
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
    ConnectionStatus connectie = connectionManager->init(setupInformation, ConnectionMode::Server);

    //Idk If we want this. Could be a problem if connectionManager Outlives the server.
    connectionManager->setOnMessageCallback(
        [this](RawMessage message){
            onMessage(message);
        });

    if (connectie == ConnectionStatus::Connected)
    {
        status = ServerStatus::Running;
    }
    else
    {
        status = ServerStatus::Error;
    }
    return status;
}

void Server::update()
{
    while (true)
    {
        connectionManager->poll();
    }
}

ServerStatus Server::stop()
{
    return status;
}

void Server::onMessage(RawMessage message)
{
    std::cout << "[Client " << message.connectionId << "]: ";
    for (size_t i = 0; i < message.length; i++) {
        std::cout << (char)message.payload[i];
    }
    std::cout << std::endl;
}

void Server::kickClient(NetworkId clientId)
{
    connectionManager->disconnect(clientId);
}

