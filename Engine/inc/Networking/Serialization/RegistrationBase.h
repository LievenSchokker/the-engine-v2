#pragma once

#include "Component/BaseComponentTypes/Component.h"
#include "Networking/Component/ComponentFactory.h"

/**
 * @brief CRTP base class for automatic component registration with the ComponentFactory.
 *
 * This class solves the problem of needing to manually register every component type
 * with the factory. Manual registration is error-prone: forgetting to register a new
 * component results in silent failures during deserialization, which are difficult to debug.
 * 
 * @tparam Derived The component class inheriting from this base (CRTP pattern)
 */
template <typename Derived>
class RegistrationBase
{
public:
    virtual ~RegistrationBase() = default;

    /**
     * @brief Provides runtime access to the component's type name.
     *
     * Virtual to allow polymorphic type identification. This enables code to query
     * a Component* for its actual type name without knowing the concrete type at
     * compile time.
     */
    [[nodiscard]] virtual const char* derivedName() const
    {
       return Derived::name();
    }

private:
    /**
     * @brief Performs the actual factory registration.
     *
     * Static function ensures registration happens exactly once per type.
     * The static_assert provides a compile-time error if someone tries to use
     * RegistrationBase with a non-Component type, catching misuse early rather
     * than failing mysteriously at runtime.
     */
    static bool registerSelf()
    {
       static_assert(std::derived_from<Derived, Component>);
       ComponentFactory::instance().registerComponent(
          Derived::name(),
          []()
          {
             return std::make_unique<Derived>();
          }
          );
       return true;
    }

    /**
     * @brief Triggers registration during static initialization.
     *
     * Inline static variable initialization guarantees registerSelf() is called
     * before main() begins.
     *
     */
    static inline bool registered = registerSelf();

    /**
     * @brief Prevents linker optimization from removing the registration.
     *
     *
     */
    virtual void forceRegistration()
    {
       (void)registered;
    }
};