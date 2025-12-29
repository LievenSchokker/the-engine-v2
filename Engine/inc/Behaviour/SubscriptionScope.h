#pragma once

#include "../Events/EventDispatcher/EventDispatcher.h"
#include "../Events/Subscriptions/SubscriptionHandle.h"

#include <vector>

/**
 * @brief RAII container for multiple event subscriptions.
 *
 * Just add as a member variable - all subscriptions automatically
 * unsubscribe when the owning object is destroyed.
 */
class SubscriptionScope
{
public:
    SubscriptionScope() = default;
    ~SubscriptionScope();

    SubscriptionScope(const SubscriptionScope&) = delete;
    SubscriptionScope& operator=(const SubscriptionScope&) = delete;
    SubscriptionScope(SubscriptionScope&& other) noexcept;
    SubscriptionScope& operator=(SubscriptionScope&& other) noexcept;

    /**
     * @brief Set the dispatcher (call once, before subscribing)
     */
    void setDispatcher(EventDispatcher& disp) { dispatcher = &disp; }

    /**
     * @brief Subscribe to an event. Automatically unsubscribes on destruction.
     */
    template <typename EventType, typename F>
    void subscribe(F&& callback);

    void unsubscribeAll();
    bool empty() const;
    size_t size() const;

private:
    EventDispatcher* dispatcher = nullptr;
    std::vector<SubscriptionHandle> handles;
};
#include "SubscriptionScope.inl"