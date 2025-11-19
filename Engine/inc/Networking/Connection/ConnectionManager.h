#pragma once


#include <functional>
#include <map>
#include <memory>


class RawMessage;
class Transport;


enum class ConnectionStatus;
enum class ConnectionMode;
enum class SendMode;
enum class TransportResult;

struct ServerConnectionInformation;
struct Connection;




class ConnectionManager
{
public:

    ConnectionManager();

    ~ConnectionManager();

    ConnectionStatus init(const ServerConnectionInformation& information, ConnectionMode connectionMode);

    static TransportResult send(int networkId, SendMode mode, const std::byte* data, size_t length);

    void disconnect(int networkId) const;

    void shutdown();
    
    void setOnMessageCallback(std::function<void(RawMessage)> callback);

    void poll();

private:

    std::unique_ptr<Transport> transport;

    std::map<int, Connection> connections;

    int maxConnections;

    ConnectionMode mode;


    void handleTransportMessage(RawMessage message);

    std::function<void(RawMessage)> onMessage;
};