#pragma once

#include "NetworkBehaviour.h"
#include "Networking/Component/ComponentFactory.h"

template<typename Derived>
class NetworkBehaviourBase : public NetworkBehaviour
{
public:
    const char* getComponentTypeName() const override
    {
        return Derived::Name();
    }

private:
    static bool RegisterSelf()
    {
        ComponentFactory::instance().registerComponent(
            Derived::Name(),
            []() { return std::make_unique<Derived>(); }
        );
        return true;
    }

    // Auto-registration happens at program startup
    static inline bool s_registered = RegisterSelf();

    // Ensures s_registered isn't optimized away
    virtual void forceRegistration() { (void)s_registered; }
};