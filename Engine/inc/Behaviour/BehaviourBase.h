#pragma once

#include "Behaviour/Behaviour.h"
#include "Networking/Component/ComponentFactory.h"

template<typename Derived>
class BehaviourBase : public Behaviour
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

    static inline bool s_registered = RegisterSelf();
    virtual void forceRegistration() { (void)s_registered; }
};