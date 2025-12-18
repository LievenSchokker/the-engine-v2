#pragma once


#include "SubscriptionScope.h"

#pragma once

template <typename EventType, typename F>
void SubscriptionScope::subscribe(F&& callback)
{
    if (dispatcher == nullptr)
        return;

    auto handle = dispatcher->subscribe<EventType>(std::forward<F>(callback));
    handles.push_back(handle);
}