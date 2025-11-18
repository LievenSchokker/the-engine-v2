#include <memory>


#include "Networking/Connection/ConnectionManager.h"
#include "Networking/Connection/ConnectionMode.h"
#include "Networking/Connection/ConnectionStatus.h"
#include "Networking/TransportGNS.h"
#include "Networking/Server/ServerInformation.h"
#include "Networking/Connection/Connection.h"
#include "Networking/RawMessage.h"


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
		result = transport->setUpListenSocket(information.port);
	}
	else
	{
		result = transport->connectByIPAdress(information.ip.c_str(), information.port);
	}

	return (result == TransportResult::SUCCES) ? ConnectionStatus::Connected : ConnectionStatus::Error;
}

TransportResult ConnectionManager::send(int networkId, SendMode mode, const std::byte *data, size_t length)
{
	TransportResult result {};

	return TransportResult::SUCCES;
}


void ConnectionManager::disconnect(int networkId) {

}

void ConnectionManager::shutdown() {
	if (transport) {
	}
	connections.clear();
}

void ConnectionManager::handleTransportMessage(RawMessage message) {
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
