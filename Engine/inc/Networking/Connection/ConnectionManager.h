#pragma once


#include <functional>
#include <map>
#include <memory>

#include "Networking/Messages/IncomingRawMessage.h"


class Transport;
class IMessage;

enum class ConnectionStatus : uint8_t;
enum class ConnectionMode;
enum class SendMode;
enum class TransportResult;

struct ServerConnectionInformation;
struct IncomingRawMessage;
class OutgoingRawMessage;
struct Connection;


class ConnectionManager
{
public:
    explicit ConnectionManager(ConnectionMode mode);

    ~ConnectionManager();

    ConnectionStatus init(const ServerConnectionInformation& information, ConnectionMode connectionMode);
    TransportResult send(const OutgoingRawMessage& message) const;

    void disconnect(int networkId) const;

    void shutdown();

    void setOnMessageCallback(std::function<void(IncomingRawMessage)> callback);
    void setOnConnectionChangedCallback(std::function<Connection> callback);

    void poll() const;

private:
    std::unique_ptr<Transport> transport;

    std::map<int, Connection> connections;

    int maxConnections;

    ConnectionMode mode;

    void handleTransportMessage(const IncomingRawMessage& message);

    std::function<void(IncomingRawMessage)> onMessage;
    std::function<void(Connection connection)> onConnectionChanged;
};
