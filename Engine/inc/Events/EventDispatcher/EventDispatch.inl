#pragma once

#include <algorithm>

#include "EventDispatcher.h"

template <typename T>
SubscriptionHandle EventDispatcher::subscribe(std::function<void(const T&)> callback)
{
    static_assert(std::is_base_of_v<EventBase, T>,
                  "T must derive from Event<T>");

    uint32_t typeId = T::staticTypeId();
    uint64_t id = nextSubscriptionId++;

    auto wrapper = [callback](EventBase& e)
    {
        callback(static_cast<T&>(e));
    };

    listeners[typeId].push_back({id, std::move(wrapper)});
    return {id, typeId};
}

template <typename T, typename F>
SubscriptionHandle EventDispatcher::subscribe(F&& callback)
{
    return subscribe<T>(
        std::function<void(const T&)>(std::forward<F>(callback)));
}

template <typename T>
void EventDispatcher::unsubscribeAll()
{
    uint32_t typeId = T::staticTypeId();
    listeners.erase(typeId);
}

template <typename T>
void EventDispatcher::dispatch(T& event)
{
    static_assert(std::is_base_of_v<EventBase, T>,
                  "T must derive from Event<T>");

    uint32_t typeId = T::staticTypeId();

    auto it = listeners.find(typeId);
    if (it != listeners.end())
    {
        for (auto& sub : it->second)
        {
            sub.callback(event);
            if (event.handled)
            {
                break;
            }
        }
    }
}

template <typename T>
void EventDispatcher::dispatch(const T& event)
{
    T copy = event;
    dispatch(copy);
}