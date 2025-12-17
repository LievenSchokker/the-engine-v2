#pragma once


#include "Component.h"
#include "Networking/Component/ComponentFactory.h"

template<typename T>
class ComponentBase : public Component
{
public:
    [[nodiscard]] const char* getComponentTypeName() const override
    {
        return T::name();
    }

private:
    static bool registerSelf()
    {
        ComponentFactory::instance().registerComponent(
            T::name(),
            []() { return std::make_unique<T>(); }
        );
        return true;
    }

    static inline bool registered = registerSelf();
    virtual void forceRegistration() { (void)registered; }
};
