#include "Behaviour/SubscriptionScope.h"

SubscriptionScope::~SubscriptionScope()
{
	unsubscribeAll();
}

SubscriptionScope::SubscriptionScope(SubscriptionScope&& other) noexcept
	: subscriptions(std::move(other.subscriptions))
{
	other.subscriptions.clear();
}

SubscriptionScope& SubscriptionScope::operator=(SubscriptionScope&& other) noexcept
{
	if (this != &other)
	{
		unsubscribeAll();
		subscriptions = std::move(other.subscriptions);
		other.subscriptions.clear();
	}
	return *this;
}

void SubscriptionScope::unsubscribeAll()
{
	for (auto& sub : subscriptions)
	{
		if (sub.dispatcher)
		{
			sub.dispatcher->unsubscribe(sub.handle);
		}
	}
	subscriptions.clear();
}

bool SubscriptionScope::empty() const
{
	return subscriptions.empty();
}

size_t SubscriptionScope::size() const
{
	return subscriptions.size();
}