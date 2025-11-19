//
// Created by thijs on 18-11-2025.
//


#pragma once
#include "Transport.h"
#include "TransportGNS.h"


class Client
{
public:
    Client();
    ~Client();
    void SetOnConnectionChanged(const OnConnectionChangedCallback& newCallback) const;
    void SetDefaultOnConnectionChanged();
    void setDefaultOnMessageReceived() const;
    void setOnMessageReceived(const OnMessageReceivedCallback& newCallback) const;
    bool connectToServer(uint16_t port, const char* serverIP);
    bool sendMessage(const std::string& text);

private:
    void createListenThread();

    std::unique_ptr<TransportGNS> transport;
    std::atomic<bool> connected = false;
    std::atomic<bool> running = false;
    int clientConnectionId = -1;
    std::thread listenThread;
};
