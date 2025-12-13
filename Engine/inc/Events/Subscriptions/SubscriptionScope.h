#pragma once

#include "Events/EventDispatcher/EventDispatcher.h"
#include "SubscriptionHandle.h"

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
	explicit SubscriptionScope(EventDispatcher& dispatcher)
		: dispatcher(&dispatcher) {}
	~SubscriptionScope();

	// Non-copyable, non-movable
	SubscriptionScope(const SubscriptionScope&) = delete;
	SubscriptionScope& operator=(const SubscriptionScope&) = delete;
	SubscriptionScope(SubscriptionScope&&) = delete;
	SubscriptionScope& operator=(SubscriptionScope&&) = delete;

	/**
	 * @brief Set the dispatcher (if not set in constructor)
	 */
	void setDispatcher(EventDispatcher& disp) { dispatcher = &disp; }

	/**
	 * @brief Subscribe to an event. Automatically unsubscribes on destruction.
	 */
	template <typename EventType, typename F>
	void subscribe(F&& callback);

	/**
	 * @brief Manually unsubscribe all.
	 */
	void unsubscribeAll();

	bool empty() const;
	size_t size() const;

private:
	EventDispatcher* dispatcher = nullptr;
	std::vector<SubscriptionHandle> handles;
};

#include "SubscriptionScope.inl"
