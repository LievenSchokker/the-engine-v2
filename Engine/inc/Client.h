//
// Created by thijs on 18-11-2025.
//
#pragma once
#include "Networking/Transport.h"
#include "Networking/TransportGNS.h"

class Client
{
public:
    Client();
    void SetOnConnectionChanged(const OnConnectionChangedCallback& newCallback) const;
    void SetDefaultOnConnectionChanged();
    void setDefaultOnMessageReceived() const;
    void setDefaultOnMessageReceived(const OnMessageReceivedCallback& newCallback) const;
    bool connectToServer(uint16_t port, const char* serverIP);
    TransportGNS* transport = new TransportGNS();
    std::atomic<bool> connected = false;
    std::atomic<bool> running = false;
    int clientConnectionId = -1;

private:
    void createInputThread(std::thread& inputThread);
    void createListenThread(std::thread& ListenThread) const;
};
