#pragma once

#include <memory>
#include <unordered_set>

#include "ServerInformation.h"
#include "Networking/SendMode.h"
#include "Networking/Server/ServerStatus.h"

class TransportGNS;
class IMessage;
class ConnectionMessage;

struct ServerConnectionInformation;
struct IncomingRawMessage;
struct Connection;

enum class ConnectionStatus : uint8_t;

class Server
{
public:
    explicit Server(const ServerConnectionInformation& serverConnectionInformation);
    ~Server();

    ServerStatus start();
    ServerStatus stop();

    void update() const;
    void kickClient(int clientId);

    bool sendMessage(int clientId, const IMessage& message, const SendMode& mode) const;
    bool sendMessage(int clientId, const IMessage& message) const;
    bool broadcastMessage(const IMessage& message) const;
    bool broadcastMessage(const IMessage& message, int excludeClientId) const;

private:
    void onMessage(const IncomingRawMessage& message);
    void onConnectionChanged(const Connection& connection);
    void handleConnectionMessage(int clientId, ConnectionMessage* message);

    std::unique_ptr<TransportGNS> transport;
    ServerConnectionInformation setupInformation;
    ServerStatus status;
    std::unordered_set<int> connectedClients;
};