#include "Networking/NetworkBuilder.h"
#include "Behaviour/NetworkBehaviour.h"

NetworkBuilder& NetworkBuilder::command(const std::string& name, Callback callback)
{
	target.addCommand(name, std::move(callback));
	return *this;
}
