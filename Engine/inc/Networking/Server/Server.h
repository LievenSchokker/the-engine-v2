#pragma once

#include <memory>
#include <unordered_set>

#include "ServerInformation.h"
#include "Networking/SendMode.h"
#include "Networking/ITransport.h"
#include "Networking/NetworkSpawnManager.h"
#include "Networking/Context/NetworkContext.h"
#include "Networking/Messages/MessageDispatcher.h"
#include "Networking/Server/ServerStatus.h"

class TransportGNS;
class IMessage;
class ConnectionMessage;

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
class Server
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
    ~Server();

    /**
     * @brief Initializes network listening and event handling
     *
     * @details Callbacks must be registered before opening socket to avoid
     *          race condition where connections arrive before handlers are ready.
     * @return Status to allow caller to handle startup failures appropriately
     */
    ServerStatus start();

    /**
     * @brief Cleanly shuts down networking and clears connection state
     *
     * @details Explicit stop allows controlled shutdown timing and proper cleanup
     *          sequencing. Client list is cleared to prevent stale state if server
     *          is restarted.
     * @return Status for caller verification
     */
    ServerStatus stop();

    /**
     * @brief Polls transport for pending network events
     *
     * @details Const because polling doesn't modify logical server state, only
     *          triggers callbacks. Must be called regularly since network I/O
     *          isn't automatically processed.
     */
    void update() const;

    /**
     * @brief Forcibly disconnects a client with notification
     *
     * @details Sends disconnect message first to allow graceful client-side
     *          cleanup before transport-level termination. Prevents client
     *          from treating kick as network error.
     */
    void kickClient(int clientId);

    void injectMessageDispatcher(std::unique_ptr<spelmotor_networking::MessageDispatcher> dispatcher);

    /**
     * @brief Sends message with explicit delivery guarantees
     *
     * @details Validates client exists to fail fast rather than silently drop
     *          messages. Explicit mode parameter for performance-critical paths
     *          where caller needs control over reliability/ordering tradeoffs.
     * @return Success flag enables caller to implement retry logic or logging
     */
    bool sendMessage(int clientId, const IMessage& message, const SendMode& mode) const;

    /**
     * @brief Convenience overload using reliable delivery by default
     *
     * @details Most messages need Unreliable; Since this is used to all clients
     *			It is also safe to assume the data is not as vital.
     *
     */
    bool sendMessage(int clientId, const IMessage& message) const;

    /**
     * @brief Sends message to all connected clients
     *
     * @details Returns aggregate success because partial broadcast failure
     *          (some clients receive, others don't) indicates inconsistent
     *          game state requiring special handling.
     */
    bool broadcastMessage(const IMessage& message) const;

    /**
     * @brief Broadcasts while excluding originating client
     *
     * @details pattern for relaying messages where sender already has
     *          the information locally. Avoids echoing data back to its source.
     */
    bool broadcastMessage(const IMessage& message, int excludeClientId) const;

    using ClientConnectedCallback = std::function<void(int clientId)>;
    using ClientDisconnectedCallback = std::function<void(int clientId)>;

    void setClientConnectedCallback(ClientConnectedCallback callback);
    void setClientDisconnectedCallback(ClientDisconnectedCallback callback)

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
     */
    void onMessage(const IncomingRawMessage& message);

    /**
     * @brief Updates connection tracking based on transport events
     *
     * @details Maintains authoritative client list separate from transport because
     *          we need fast lookup for message validation and transport state may
     *          include transient connections not yet fully established.
     */
    void onConnectionChanged(const Connection& connection);

    /**
     * @brief Handles client-initiated disconnection requests
     *
     * @details Separated to distinguish graceful client disconnects from errors
     *          or kicks, enabling appropriate cleanup logic for each scenario.
     */
    void handleConnectionMessage(int clientId, ConnectionMessage* message);

    /// @brief Abstracted transport layer for testing and multi-backend support
    std::unique_ptr<ITransport> transport;

    /// @brief Cached for potential restart without requiring reconfiguration
    ServerConnectionInformation setupInformation;

    /// @brief Tracks lifecycle state for preventing invalid operations
    ServerStatus status;

    /// @brief Fast lookup set for validating message sources and broadcast targets
    std::unordered_set<int> connectedClients;

    std::unique_ptr<spelmotor_networking::MessageDispatcher> messageDispatcher;

    std::unique_ptr<NetworkContext> networkContext;

    NetworkSpawnManager* spawnManage;

    ClientConnectedCallback onClientConnected;
    ClientDisconnectedCallback onClientDisconnected;

};

