#include "Networking/Server/Server.h"

#include <iostream>

#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"

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
    ConnectionStatus connnectionStatus = connectionManager->init(setupInformation, ConnectionMode::Server);

    //Idk If we want this. Could be a problem if connectionManager Outlives the server.
    connectionManager->setOnMessageCallback(
        [this](RawMessage message){
            onMessage(message);
        });

    if (connnectionStatus == ConnectionStatus::Connected)
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
    //If Anything happens callback will link back to server
    connectionManager->poll();
}

ServerStatus Server::stop()
{
    return status;
}

void Server::onMessage(RawMessage message)
{
    //TODO MESSAGE SYSTEM
    std::cout << "[Client " << message.getConnectionID() << "]: ";
    for (size_t i = 0; i < message.getLength(); i++) {
        std::cout << (char)message.getPayload()[i];
    }
    std::cout << std::endl;
}

void Server::kickClient(int clientId)
{
    //TODO KICK CLIENT MESSAGE
}

