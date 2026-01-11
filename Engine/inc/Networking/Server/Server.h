#pragma once


#include <memory>
#include <unordered_set>
#include <functional>

#include "ServerInformation.h"
#include "StateSyncSystem.h"
#include "Core/IEngineSystems.h"
#include "Networking/SendMode.h"
#include "Networking/ITransport.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Messages/MessageDispatcher.h"

class TransportGNS;
class IMessage;
class ConnectionMessage;
struct ApplicationSpecifications;
struct ServerConnectionInformation;
struct IncomingRawMessage;
struct Connection;

enum class ConnectionStatus : uint8_t;

/**
 * @brief Manages game server networking with abstracted transport layer
 *
 * @details Designed to decouple high-level server logic from transport implementation.
 *          Maintains authoritative list of connected clients for validation and routing.
 */
class Server : public IEngineSystems
{
public:
    /**
     * @brief Constructs server with dependency-injected transport
     *
     * @details Transport is injected to enable unit testing with mock transports
     *          and runtime selection of different network backends (Steam, EOS, etc.)
     *          without recompiling server logic.
     */
    Server(const ServerConnectionInformation& serverConnectionInformation,
           std::unique_ptr<ITransport> injectedTransport);

    /**
     * @brief Ensures graceful shutdown even if stop() wasn't called explicitly
     *
     * @details Prevents resource leaks and dangling connections when Server
     *          goes out of scope or is destroyed during error handling.
     */
    ~Server() override;

    /**
     * @brief Initializes network listening and event handling
     *
     * @details Callbacks must be registered before opening socket to avoid
     *          race condition where connections arrive before handlers are ready.
     * @param gameWorld Reference to the game world for system registration.
     * @return Status to allow caller to handle startup failures appropriately
     */
    SystemStatus start(GameWorld& gameWorld) override;

    /**
     * @brief Polls transport for pending network events
     *
     * @details Must be called regularly since network I/O isn't automatically processed.
     * @param deltaTime Time since last update.
     * @param gameWorld Reference to the game world.
     */
    void update(double deltaTime, const GameWorld& gameWorld) override;

    /**
     * @brief Advances the server tick loop for state sync.
     *
     * @param deltaTime Time since last fixed update.
     * @param gameWorld Reference to the game world.
     */
    void fixedUpdate(double deltaTime, const GameWorld& gameWorld) override;

    /**
     * @brief Cleanly shuts down networking and clears connection state
     *
     * @details Explicit stop allows controlled shutdown timing and proper cleanup
     *          sequencing. Client list is cleared to prevent stale state if server
     *          is restarted.
     * @param gameWorld Reference to the game world for system deregistration.
     */
    void shutdown(GameWorld& gameWorld) override;

    /**
     * @brief Returns the name of this system.
     * @return System name string.
     */
    [[nodiscard]] const std::string getName() const override;

    /**
     * @brief Forcibly disconnects a client with notification
     *
     * @details Sends disconnect message first to allow graceful client-side
     *          cleanup before transport-level termination. Prevents client
     *          from treating kick as network error.
     * @param clientId The client to disconnect.
     */
    void kickClient(int clientId);

    /**
     * @brief Injects a message dispatcher for handling incoming messages.
     * @param dispatcher The message dispatcher to use.
     */
    void injectMessageDispatcher(std::unique_ptr<spelmotorNetworking::MessageDispatcher> dispatcher);

    /**
     * @brief Sends message with explicit delivery guarantees
     *
     * @details Validates client exists to fail fast rather than silently drop
     *          messages. Explicit mode parameter for performance-critical paths
     *          where caller needs control over reliability/ordering tradeoffs.
     * @param clientId Target client.
     * @param message Message to send.
     * @param mode Delivery mode (reliable, unreliable, etc.)
     * @return Success flag enables caller to implement retry logic or logging
     */
    bool sendMessage(int clientId, const IMessage& message, const SendMode& mode) const;

    /**
     * @brief Convenience overload using unreliable delivery by default
     *
     * @details Most messages need Unreliable; Since this is used to all clients
     *          it is also safe to assume the data is not as vital.
     * @param clientId Target client.
     * @param message Message to send.
     * @return Success flag.
     */
    bool sendMessage(int clientId, const IMessage& message) const;

    /**
     * @brief Sends message to all connected clients
     *
     * @details Returns aggregate success because partial broadcast failure
     *          (some clients receive, others don't) indicates inconsistent
     *          game state requiring special handling.
     * @param message Message to broadcast.
     * @return True if all sends succeeded.
     */
    bool broadcastMessage(const IMessage& message) const;

    /**
     * @brief Broadcasts while excluding originating client
     *
     * @details Pattern for relaying messages where sender already has
     *          the information locally. Avoids echoing data back to its source.
     * @param message Message to broadcast.
     * @param excludeClientId Client to exclude from broadcast.
     * @return True if all sends succeeded.
     */
    bool broadcastMessage(const IMessage& message, int excludeClientId) const;

    using ClientConnectedCallback = std::function<void(int clientId)>;
    using ClientDisconnectedCallback = std::function<void(int clientId)>;

    /**
     * @brief Sets callback for when a client connects.
     * @param callback Function to call on client connection.
     */
    void setClientConnectedCallback(ClientConnectedCallback callback);

    /**
     * @brief Sets callback for when a client disconnects.
     * @param callback Function to call on client disconnection.
     */
    void setClientDisconnectedCallback(ClientDisconnectedCallback callback);

    /**
     * @brief Converts application specifications to server connection information.
     * @param specifications Application specifications containing network settings.
     * @return ServerConnectionInformation struct.
     */
    static ServerConnectionInformation convertApplicationSettings(const ApplicationSpecifications& specifications);

	int getConnectedClientCount() const;
	const std::unordered_set<int>& getConnectedClientIds() const;
private:
    /**
     * @brief Deserializes and routes incoming messages
     *
     * @details Separated from transport layer to centralize message validation
     *          and provide single point for security checks. Ignores unknown
     *          clients to prevent processing messages from half-connected or
     *          malicious sources. Does not validate clientId besides checking
     *          if it exists. Security Checks should happen in transport layer.
     *          This way our server can remain kind of dumb.
     * @param message The raw incoming message.
     */
    void onMessage(const IncomingRawMessage& message);

    /**
     * @brief Updates connection tracking based on transport events
     *
     * @details Maintains authoritative client list separate from transport because
     *          we need fast lookup for message validation and transport state may
     *          include transient connections not yet fully established.
     * @param connection The connection that changed.
     */
    void onConnectionChanged(const Connection& connection);

    /**
     * @brief Handles client-initiated disconnection requests
     *
     * @details Separated to distinguish graceful client disconnects from errors
     *          or kicks, enabling appropriate cleanup logic for each scenario.
     * @param clientId The client that sent the message.
     * @param message The connection message.
     */
    void handleConnectionMessage(int clientId, ConnectionMessage* message);

    /**
     * @brief Handles new client connection setup.
     * @param clientId The newly connected client.
     */
    void handleNewClientConnected(int clientId) const;

    std::unique_ptr<ITransport> transport;
    ServerConnectionInformation setupInformation;
    SystemStatus status;
    std::unordered_set<int> connectedClients;
    std::unique_ptr<spelmotorNetworking::MessageDispatcher> messageDispatcher;
    ClientConnectedCallback onClientConnected;
    ClientDisconnectedCallback onClientDisconnected;
	std::unique_ptr<StateSyncSystem> stateSyncSystem;
};
