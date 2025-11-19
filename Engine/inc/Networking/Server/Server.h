// Server.h
#pragma once

#include <memory>
#include <unordered_set>

#include "ServerInformation.h"
#include "Networking/Server/ServerStatus.h"

class ConnectionManager;
class IMessage;
class ConnectionMessage;
class PlayerMoveMessage;

struct ServerConnectionInformation;
struct IncomingRawMessage;

enum class ConnectionMode;
enum class ConnectionStatus : uint8_t;

class Server
{
public:
    explicit Server(const ServerConnectionInformation& serverConnectionInformation);
    ~Server();

    ServerStatus start();
    ServerStatus stop();

    void update();
    void kickClient(int clientId);

private:
    void onMessage(IncomingRawMessage message);
    void handleConnectionMessage(int clientId, ConnectionMessage* message);

    std::unique_ptr<ConnectionManager> connectionManager;
    ServerConnectionInformation setupInformation;
    ServerStatus status;
    std::unordered_set<int> connectedClients;
};
