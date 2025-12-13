

#include "Events/Subscriptions/SubscriptionScope.h"
SubscriptionScope::~SubscriptionScope()
{
    unsubscribeAll();
}

void SubscriptionScope::unsubscribeAll()
{
    if (dispatcher == nullptr)
        return;

    for (auto& handle : handles)
    {
        dispatcher->unsubscribe(handle);
    }
    handles.clear();
}

bool SubscriptionScope::empty() const
{
    return handles.empty();
}

size_t SubscriptionScope::size() const
{
    return handles.size();
}