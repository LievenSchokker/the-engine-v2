#include "Networking/Component/ComponentFactory.h"
#include "Component/BaseComponentTypes/Component.h"

ComponentFactory& ComponentFactory::instance()
{
	static ComponentFactory factory;
	return factory;
}

bool ComponentFactory::isRegistered(const std::string& name) const
{
	return creators.contains(name);
}

std::unique_ptr<Component> ComponentFactory::create(
	const std::string& name) const
{
	auto componentEntry = creators.find(name);
	if (componentEntry == creators.end()) return nullptr;
	return componentEntry->second();
}

bool ComponentFactory::registerComponent(const char* name, Creator creator)
{
	std::string nameStr(name);
	if (creators.contains(nameStr))
	{
		std::abort();
	}
	creators[nameStr] = std::move(creator);
	return true;
}