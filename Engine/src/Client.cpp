//
// Created by thijs on 18-11-2025.
//


#include "../inc/Client.h"
#include <iostream>


Client::Client()
{
    transport = std::make_unique<TransportGNS>();
    setDefaultOnMessageReceived();
    SetDefaultOnConnectionChanged();
}


Client::~Client()
{
    running = false;
    transport->closeOpenSocket();

    if (listenThread.joinable()) listenThread.join();
}


void Client::SetOnConnectionChanged(const OnConnectionChangedCallback& newCallBack) const
{
    transport->setOnConnectionChanged(newCallBack);
}


void Client::SetDefaultOnConnectionChanged()
{
    transport->setOnConnectionChanged([&](int assignedId, bool isConnected)
    {
        if (isConnected)
        {
            connected = true;
            clientConnectionId = assignedId;

            std::cout << "[Client] Connected. Assigned ID: "
                << clientConnectionId << "\n";
            std::cout << "Type messages. /quit to disconnect.\n";
        }
        else
        {
            connected = false;
            running = false;
            std::cout << "[Client] Disconnected.\n";
        }
    });
}


void Client::setOnMessageReceived(const OnMessageReceivedCallback& newCallback) const
{
    transport->setOnMessageReceived(newCallback);
}


void Client::setDefaultOnMessageReceived() const
{
    transport->setOnMessageReceived([&](const RawMessage& message)
    {
        std::cout << "[Server]: " << message.toString() << std::endl;
    });
}


bool Client::connectToServer(const uint16_t port, const char* serverIP)
{
    TransportResult transportResult = transport->connectByIPAdress(serverIP, port);
    if (transportResult == TransportResult::ERROR)
    {
        std::cerr << "[Client] Failed to initiate connection.\n";
        return false;
    }

    running = true;
    createListenThread();

    return true;
}

bool Client::sendMessage(const std::string& text)
{
    if (!connected) return false;

    RawMessage message(1, text);
    transport->send(message);

    return true;
}



// #TODO New thread created
void Client::createListenThread()
{
    listenThread = std::thread([this]()
    {
        while (running)
        {
            transport->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}
