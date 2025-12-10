#pragma once


#include "../IMessage.h"

#include <string>
#include <vector>
#include <cstdint>

#include "Networking/Serialization/Serialization.h"

/**
 * @class ActionMessage
 * @brief Encapsulates an RPC command for network transmission.
 *
 * ActionMessage exists to decouple game logic from network transport. Rather than
 * having components directly invoke methods across the network, they emit ActionMessages
 * that can be queued, batched, validated, and replayed independently of execution.
 *
 *
 */
class ActionMessage : public IMessage {
public:

    //Need default construction for deserialization.
    ActionMessage() = default;

    /**
     * @brief Constructs a fully-specified action ready for transmission.
     *
     * All fields are required at construction because partial ActionMessages
     * are meaningless—an action without a target or timing cannot be executed.
     * This forces callers to provide complete context upfront rather than
     * risking malformed messages entering the network pipeline.
     */
    ActionMessage(uint32_t compId, uint32_t objId, std::string action, uint32_t tick);

    /**
     * @brief Converts the message to a byte stream for network transmission.
     *
     *
     * Used by messageWriter to serialize into RawMessageFormat
     */
    std::vector<std::byte> serialize() const;

    /**
     * @brief Reconstructs the message from received network data.
     *
     * Returns success/failure rather than throwing because deserialization
     * failures are expected during normal operation (corrupted packets,
     * version mismatches, malicious input). Callers can handle failures
     * gracefully without exception overhead in the hot path.
     *
     */
    bool deserialize(const std::byte* data, size_t length);

    /**
     * @brief Checks whether this message represents a coherent, executable action.
     *
     */
    bool validate() const;

    /**
     * @brief Identifies which component on the target object should handle this action.
     *
     */
    uint32_t getComponentIdentity() const;

    /**
     * @brief Identifies which game object this action targets.
     *
     */
    uint32_t getGameObjectIdentity() const;

    /**
     * @brief The action to perform, as a string key.
     *
     */
    const std::string& getAction() const;

    /**
     * @brief The simulation tick when this action should be applied.
     *
     */
    uint32_t getTick() const;

    /// @name Mutators
    /// @{
    /**
     * Mutators exist primarily for deserialization and message pooling/reuse.
     * Prefer constructing complete messages when possible.
     */
    void setComponentIdentity(uint32_t id);
    void setGameObjectIdentity(uint32_t id);
    void setAction(std::string action);
    void setTick(uint32_t tick);
    /// @}

private:
    uint32_t networkComponentIdentity;  ///< Routes to the handling component
    uint32_t networkGameObjectIdentity; ///< Routes to the target entity
    std::string actionKey;              ///< Identifies the RPC to invoke
    uint32_t tick;                      ///< Simulation frame for temporal ordering
};