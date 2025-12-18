#pragma once

#include "Networking/MessageHandlers/IMessageHandler.h"

enum class MessageTypes : uint8_t;
class IMessage;

#include <memory>
#include <unordered_map>

namespace spelmotorNetworking
{
/**
 * @brief Routes network messages to registered handlers.
 *
 * Maps each MessageType to a single IMessageHandler. Handlers are
 * registered at startup and invoked when matching messages arrive.
 * Takes ownership of all registered handlers.
 */
class MessageDispatcher
{
public:
	virtual ~MessageDispatcher() = default;

	/**
	 * @brief Routes message to its handler, transferring ownership.
	 * drops unknown types for forward compatibility.
	 */
	void processMessage(std::unique_ptr<IMessage> message);

	/**
	 * @brief Registers a handler. Returns false if type already registered.
	 */
	bool registerMessageHandler(MessageTypes type,
								std::unique_ptr<IMessageHandler> handler);

	/**
	 * @brief Removes handler. Returns false if none existed.
	 */
	bool deregisterMessageHandler(MessageTypes type);

private:
	IMessageHandler* getMessageHandler(MessageTypes type);

	std::unordered_map<MessageTypes, std::unique_ptr<IMessageHandler>>
		messageHandlers;
};
}