#pragma once

#include "UIRenderComponent.h"
#include "Networking/Component/ComponentFactory.h"

template<typename Derived>
class UserInterfaceRenderComponentBase : public UserInterfaceRenderComponent
{
public:
    const char* getComponentTypeName() const override
    {
        return Derived::name();
    }

private:
    static bool registerSelf()
    {
        ComponentFactory::instance().registerComponent(
            Derived::name(),
            []() { return std::make_unique<Derived>(); }
        );
        return true;
    }

    static inline bool registered = registerSelf();
    virtual void forceRegistration() { (void)registered; }
};
