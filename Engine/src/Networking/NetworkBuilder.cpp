#include "Networking/NetworkBuilder.h"
#include "Behaviour/NetworkBehaviour.h"

NetworkBuilder& NetworkBuilder::command(const std::string& name,
                                        Callback callback)
{
	target.commands[name] = std::move(callback);
	return *this;
}

NetworkBuilder& NetworkBuilder::clientRpc(const std::string& name,
                                          Callback callback)
{
	target.rpcs[name] = std::move(callback);
	return *this;
}