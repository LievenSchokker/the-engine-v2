#pragma once


#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

class Component;

class ComponentFactory
{
public:
    using Creator = std::function<std::unique_ptr<Component>()>;

    static ComponentFactory& instance()
    {
        static ComponentFactory factory;
        return factory;
    }

    bool registerComponent(const char* name, Creator creator)
    {
        std::cout << name << " registered" << std::endl;
        std::string nameStr(name);
        if (creators.contains(nameStr))
        {
            std::cerr << "FATAL: Duplicate component '" << name << "'" << std::endl;
            std::abort();
        }
        creators[nameStr] = std::move(creator);
        return true;
    }

    [[nodiscard]] std::unique_ptr<Component> create(const std::string& name) const
    {
        auto it = creators.find(name);
        if (it == creators.end()) return nullptr;
        return it->second();
    }

    bool isRegistered(const std::string& name) const
    {
        return creators.contains(name);
    }

private:
    ComponentFactory() = default;
    std::unordered_map<std::string, Creator> creators;
};