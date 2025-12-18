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

	static ComponentFactory& instance();

	bool registerComponent(const char* name, Creator creator);

	[[nodiscard]] std::unique_ptr<Component> create(const std::string& name) const;

	bool isRegistered(const std::string& name) const;

private:
	ComponentFactory() = default;
	std::unordered_map<std::string, Creator> creators;
};