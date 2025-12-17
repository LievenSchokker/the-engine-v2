#pragma once

#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Networking/Component/ComponentFactory.h"

template<typename Derived>
class RenderComponentBase : public RenderComponent
{
public:
    [[nodiscard]] const char* getComponentTypeName() const override
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