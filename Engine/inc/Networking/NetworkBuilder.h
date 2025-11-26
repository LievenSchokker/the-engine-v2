#pragma once

#include "Networking/Serialization/Serialization.h"
#include <functional>
#include <string>

class NetworkBehaviour;

/**
 * @brief Fluent builder for registering network methods on NetworkBehaviour.
 */
class NetworkBuilder
{
public:
    explicit NetworkBuilder(NetworkBehaviour& behaviour) : target(behaviour) {}

    using Callback = std::function<void(ReadArchive&)>;

    // Register a client→server command
    NetworkBuilder& command(const std::string& name, Callback callback);

    // Register a server→client RPC
    NetworkBuilder& clientRpc(const std::string& name, Callback callback);

private:
    NetworkBehaviour& target;
};