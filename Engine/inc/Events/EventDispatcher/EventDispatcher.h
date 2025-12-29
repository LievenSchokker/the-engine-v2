#pragma once

#include "../Subscriptions/SubscriptionHandle.h"
#include "Events/Event.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <cstdint>

/**
 * @brief Central event dispatcher for type-safe event handling
 *
 * Supports:
 * - Type-safe subscriptions via templates
 * - Subscription handles for unsubscribing
 * - Event consumption (handled flag stops propagation)
 */
class EventDispatcher
{
public:
    EventDispatcher() = default;
    ~EventDispatcher() = default;

    /**
     * @brief Subscribe to an event type
     * @tparam T Event type to subscribe to
     * @param callback Function to call when event is dispatched
     * @return Handle for unsubscribing
     */
    template <typename T>
    SubscriptionHandle subscribe(std::function<void(const T&)> callback);

    /**
     * @brief Subscribe with a lambda (convenience overload)
     */
    template <typename T, typename F>
    SubscriptionHandle subscribe(F&& callback);

    /**
     * @brief Unsubscribe using a handle
     */
    void unsubscribe(SubscriptionHandle& handle);

    /**
     * @brief Unsubscribe all listeners for a specific event type
     */
    template <typename T>
    void unsubscribeAll();

    /**
     * @brief Unsubscribe all listeners for all event types
     */
    void unsubscribeAll();

    /**
     * @brief Dispatch an event to all subscribers
     * @tparam T Event type
     * @param event Event to dispatch
     *
     * Stops dispatching if event.handled becomes true
     */
    template <typename T>
    void dispatch(T& event);

    /**
     * @brief Dispatch an event (const overload, creates copy)
     */
    template <typename T>
    void dispatch(const T& event);

private:
    using WrapperCallback = std::function<void(EventBase&)>;

    struct Subscription
    {
        uint64_t id;
        WrapperCallback callback;
    };

    std::unordered_map<uint32_t, std::vector<Subscription>> listeners;
    uint64_t nextSubscriptionId = 1;
};

#include "EventDispatch.inl"
