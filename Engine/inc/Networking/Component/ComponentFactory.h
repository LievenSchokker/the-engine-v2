#pragma once

#include "Networking/ComponentType.h"

#include <functional>
#include <memory>
#include <unordered_map>

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

	void registerComponent(ComponentType type, Creator creator)
	{
		creators[type] = std::move(creator);
	}

	std::unique_ptr<Component> create(ComponentType type) const
	{
		auto it = creators.find(type);
		if (it == creators.end()) return nullptr;
		return it->second();
	}

	bool isRegistered(ComponentType type) const
	{
		return creators.contains(type);
	}

private:
	ComponentFactory() = default;
	std::unordered_map<ComponentType, Creator> creators;
};