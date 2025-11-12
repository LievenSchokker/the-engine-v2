#pragma once
#include <cstddef>
#include <functional>
#include "SendMode.h"

using OnMessageReceivedCallback = std::function<void(int connectionId, const std::byte* data, size_t length)>;
using OnConnectionChangedCallback = std::function<void(int connectionId, bool connected)>;

class Transport {
public:
	virtual ~Transport() = default;

	virtual TransportResult startServer(uint16_t port) = 0;
	virtual TransportResult startClient(const char* serverAddress, uint16_t port) = 0;
	virtual TransportResult send(int connectionId, const std::byte data[], size_t length, bool reliable) = 0;
	virtual bool disconnect(int connectionId) = 0;
	virtual void poll() = 0;
	virtual void shutdown() = 0;

	void setOnMessageReceived(OnMessageReceivedCallback callback) { onMessageReceived = callback; }
	void setOnConnectionChanged(OnConnectionChangedCallback callback) { onConnectionChanged = callback; }

protected:
	OnMessageReceivedCallback onMessageReceived;
	OnConnectionChangedCallback onConnectionChanged;
};