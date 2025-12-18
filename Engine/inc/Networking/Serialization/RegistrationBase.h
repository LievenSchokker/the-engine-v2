#pragma once


#include "Component/BaseComponentTypes/Component.h"
#include "Networking/Component/ComponentFactory.h"

template <typename Derived>
class RegistrationBase
{
public:
	virtual ~RegistrationBase() = default;

	[[nodiscard]] virtual const char* derivedName() const
	{
		return Derived::name();
	}

private:
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

	static inline bool registered = registerSelf();

	virtual void forceRegistration()
	{
		(void)registered;
	}
};