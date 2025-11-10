#pragma once
#include <cstddef>
#include <vector>
#include <functional>

enum class SendMode {
    ReliableOrdered,
    ReliableUnordered,
    Unreliable
};

struct TransportResult {
    bool success;
    int errorCode;
};

using OnMessageReceivedCallback = std::function<void(int connectionId, const std::byte* data, size_t length)>;
using OnConnectionChangedCallback = std::function<void(int connectionId, bool connected)>;

class Transport {
public:
	virtual ~Transport() = default;

	virtual TransportResult StartServer(uint16_t port) = 0;
	virtual TransportResult StartClient(const char* serverAddress, uint16_t port) = 0;
	virtual TransportResult Send(int connectionId, const std::byte data[], size_t length, bool reliable) = 0;
	virtual TransportResult Send(int connectionId, const std::byte data[], size_t length, SendMode mode) = 0;
	virtual TransportResult Disconnect(int connectionId) = 0;
	virtual void Poll() = 0;
	virtual void Shutdown() = 0;

	void SetOnMessageReceived(OnMessageReceivedCallback callback) { m_onMessageReceived = callback; }
	void SetOnConnectionChanged(OnConnectionChangedCallback callback) { m_onConnectionChanged = callback; }

protected:
	OnMessageReceivedCallback m_onMessageReceived;
	OnConnectionChangedCallback m_onConnectionChanged;
};