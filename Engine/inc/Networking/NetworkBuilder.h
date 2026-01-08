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

    NetworkBuilder& command(const std::string& name, Callback callback);

	template<typename T, typename... Args>
	void command(const std::string& name, void (T::*method)(Args...));
private:
    NetworkBehaviour& target;
};

#include "NetworkBuilder.inl"