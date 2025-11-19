// Client.h
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

class ConnectionManager;
class IMessage;
struct IncomingRawMessage;
struct ServerConnectionInformation;

using OnMessageReceivedCallback = std::function<void(const IncomingRawMessage&)>;
using OnConnectionChangedCallback = std::function<void(int, bool)>;

class Client
{
public:
    Client();
    ~Client();

    bool connectToServer(uint16_t port, const char* serverIP);
    bool sendMessage(IMessage& message);
    bool isConnected() const { return connected; }
    int getConnectionId() const { return clientConnectionId; }

    void setOnMessageReceived(const OnMessageReceivedCallback& callback);
    void setOnConnectionChanged(const OnConnectionChangedCallback& callback);

private:
    std::unique_ptr<ConnectionManager> connectionManager;
    std::thread listenThread;
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};
    int clientConnectionId{-1};

    void createListenThread();
    void setDefaultOnMessageReceived();
    void setDefaultOnConnectionChanged();
};
