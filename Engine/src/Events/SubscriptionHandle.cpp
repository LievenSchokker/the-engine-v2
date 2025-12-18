#include "../../inc/Events/Subscriptions/SubscriptionHandle.h"

SubscriptionHandle::SubscriptionHandle(const uint64_t id, const uint32_t typeId) :
	id(id),
	typeId(typeId)
{
}

bool SubscriptionHandle::isValid() const
{
	return id != 0;
}

void SubscriptionHandle::invalidate()
{
	id = 0;
}
