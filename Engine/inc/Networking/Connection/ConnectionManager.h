#pragma once


#include <map>
#include <memory>

#include "core/Networking/RawMessage.h"


class RawMessage;
class Transport;


enum class ConnectionStatus;
enum class ConnectionMode;
enum class TransportResult;
enum class SendMode;


struct ServerConnectionInformation;
struct Connection;
struct NetworkId;



class ConnectionManager
{
public:

    ConnectionManager();

    ~ConnectionManager();

    ConnectionStatus init(const ServerConnectionInformation& information, ConnectionMode connectionMode);

    TransportResult send(NetworkId id, SendMode mode, const std::byte *data, size_t length);

    void disconnect(NetworkId id);

    void shutdown();

    void setOnMessageCallback(std::function<void(RawMessage)> callback);

    void poll();

private:

    std::unique_ptr<Transport> transport;

    std::map<NetworkId, Connection> connections;

    int maxConnections;

    ConnectionMode mode;


    void handleTransportMessage(RawMessage message);

    std::function<void(RawMessage)> onMessage;
};