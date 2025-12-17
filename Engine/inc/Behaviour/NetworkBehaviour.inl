#pragma once


#include "NetworkBehaviour.h"
#include "Networking/Component/ComponentFactory.h"
#include "Core/GameWorld.h"

template<typename... Args>
void NetworkBehaviour::callCommand(const std::string& name, Args&&... args)
{
	if (!isClient() || !identity)
	{
		return;
	}

	if (!world)
	{
		return;
	}

	const ActionMessage message(
		componentNetworkId,
		identity->getNetId(),
		name,
		0
	);

	world->sendToServer(message);
}

inline void NetworkBehaviour::addCommand(const std::string& name, ActionCallback callback)
{
	commands[name] = std::move(callback);
}

inline AuthorityType NetworkBehaviour::getAuthorityType() const
{
    return authorityType;
}

template<typename Derived>
class NetworkBehaviourImpl : public NetworkBehaviour
{
public:
    const char* getComponentTypeName() const override
    {
        return Derived::Name();
    }

protected:
    static bool RegisterSelf()
    {
        return ComponentFactory::instance().registerComponent(
            Derived::Name(),
            []() { return std::make_unique<Derived>(); }
        );
    }

private:
    static inline bool s_registered = RegisterSelf();
    int forceRegistration() { return s_registered ? 1 : 0; }
};