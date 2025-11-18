#include <memory>


#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/NetworkID.h"
#include "Networking/Connection/Connection.h"


ConnectionManager::ConnectionManager()
	: transport(std::make_unique<TransportGNS>()),
      maxConnections(100),
      mode(ConnectionMode::Server)
{
}
ConnectionManager::~ConnectionManager() = default;

ConnectionStatus ConnectionManager::init(const ServerConnectionInformation& information,
									  ConnectionMode connectionMode) {
	mode = connectionMode;
	TransportResult result{};

	if (!transport) {
		return ConnectionStatus::Error;
	}


	transport->setOnMessageReceived(
		[this](RawMessage message){
		handleTransportMessage(message);
	});

	if (mode == ConnectionMode::Server)
	{
		result = transport->startServer(information.port);
	}
	else
	{
		result = transport->startClient(information.ip.c_str(), information.port);
	}

	return (result == TransportResult::SUCCESS) ? ConnectionStatus::Connected : ConnectionStatus::Error;
}

TransportResult ConnectionManager::send(NetworkId id, SendMode mode, const std::byte *data, size_t length)
{
	TransportResult result {};
	int transportId = connections.begin()->second.transportConnectionId;

	RawMessage message;
	message.connectionId = id.networkId;
	message.mode = mode;
	std::vector<std::byte> vector;
	vector.assign(data, data + length);
	message.payload = vector;
	if (connections.empty())
	{
		return TransportResult::ERROR;
	}

	auto messageReceiver = connections.find(id);

	if (messageReceiver != connections.end())
	{
		return transport->send(message);
	}

	return TransportResult::SUCCESS;
}


void ConnectionManager::disconnect(NetworkId networkId) {
	auto Connection = connections.find(networkId);

	if (Connection != connections.end()) {
		transport->disconnect(networkId.networkId);
		connections.erase(Connection);
	}
}

void ConnectionManager::shutdown() {
	if (transport) {
		transport->shutdown();
	}
	connections.clear();
}

void ConnectionManager::handleTransportMessage(RawMessage message) {
	NetworkId networkId{};
	networkId.networkId = message.connectionId;


	auto Connection = connections.find(networkId);

	if (Connection == connections.end())
	{
		return;
	}

	if (onMessage != nullptr)
	{
		onMessage(message);
	}
}

void ConnectionManager::setOnMessageCallback(
	std::function<void(RawMessage)> callback)
{
	onMessage = std::move(callback);
}

void ConnectionManager::poll()
{
	if (transport)
	{
		transport->poll();
	}
}
