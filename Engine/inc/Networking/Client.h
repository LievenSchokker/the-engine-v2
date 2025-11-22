#pragma once

#include <memory>

#include "Connection/Connection.h"

class TransportGNS;
class IMessage;

struct IncomingRawMessage;

class Client
{
public:
    Client();
    ~Client();

    bool connectToServer(uint16_t port, const char* serverIP);
    void disconnect();

    bool sendMessage(IMessage& message);
    void poll();

    bool isConnected() const;

private:
    void onMessageReceived(const IncomingRawMessage& rawMessage);
    void onConnectionChanged(const Connection& connection);

    std::unique_ptr<TransportGNS> transport;
    Connection currentConnection{};
};