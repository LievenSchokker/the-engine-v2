// Client.h
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include "Connection/Connection.h"

class ConnectionManager;
class IMessage;
struct IncomingRawMessage;
struct ServerConnectionInformation;

class Client
{
public:
    Client();
    ~Client();

    bool connectToServer(uint16_t port, const char* serverIP);
    bool sendMessage(IMessage& message);

private:
    Connection currentConnection{};
    std::unique_ptr<ConnectionManager> connectionManager;
    std::thread listenThread;

    static void onMessageReceived(const IncomingRawMessage& rawMessage);
    void onConnectionChanged();
    void poll();
};
