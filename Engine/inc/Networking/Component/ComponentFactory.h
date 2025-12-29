#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

class Component;

/*
 *
 * This factory exists to support network serialization of GameObjects. When a GameObject
 * is received over the network, we only have the component type as a string
 *
 * Singleton pattern is used because:
 * - Component registration happens at static initialization time (before main)
 * - Registered components must be available globally for deserialization
 * - Multiple factory instances would fragment registrations, causing lookup failures
 *
 */
class ComponentFactory
{
public:
    using Creator = std::function<std::unique_ptr<Component>()>;

    /**
     * @brief Returns the global factory instance.
     * the factory is guaranteed to exist when
     * RegistrationBase static initializers call registerComponent().
     */
    static ComponentFactory& instance();

    /**
     * @brief Registers a component type with its factory function.
     *
     * @param name Type identifier used during serialization (must match Derived::name())
     * @param creator Lambda that default-constructs the component
     * @return true on success (return value exists for static init pattern)
     */
    bool registerComponent(const char* name, Creator creator);

    /**
     * @brief Instantiates a component by its registered type name.
     *
     * Primary use case is deserialization: after reading a type name from the
     * network stream, this creates the corresponding component instance which
     * can then deserialize its own state.
     *
     * @param name The type identifier to look up
     * @return New component instance, or nullptr if type is unregistered
     */
    [[nodiscard]] std::unique_ptr<Component> create(const std::string& name) const;

    /**
     * @brief Checks if a component type can be serialized.
     *
     * Used during serialization to skip components that weren't registered
     * (e.g., engine-internal components not intended for network sync).
     * Prevents serializing data that the receiving end couldn't deserialize.
     */
    bool isRegistered(const std::string& name) const;

private:
    /**
     * Private constructor enforces singleton access through instance().
     */
    ComponentFactory() = default;

    /**
     * Maps type names to their factory functions. Uses std::string keys for
     * safe storage (const char* from static strings could theoretically be
     * invalidated in plugin/DLL scenarios).
     */
    std::unordered_map<std::string, Creator> creators;
};