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

	// Can't allow copy since that would defeat the purpose of the ownership
	SubscriptionScope(const SubscriptionScope&) = delete;
	SubscriptionScope& operator=(const SubscriptionScope&) = delete;

	// Move allowed
	SubscriptionScope(SubscriptionScope&& other) noexcept;
	SubscriptionScope& operator=(SubscriptionScope&& other) noexcept;

	/**
	 * @brief Subscribe to an event. Automatically unsubscribes on destruction.
	 */
	template <typename EventType, typename F>
	void subscribe(EventDispatcher& dispatcher, F&& callback);

	/**
	 * @brief Manually unsubscribe all.
	 */
	void unsubscribeAll();

	/**
	 * @brief Check if there are active subscriptions.
	 */
	bool empty() const;

	/**
	 * @brief Get number of active subscriptions.
	 */
	size_t size() const;

private:
	struct Entry
	{
		EventDispatcher* dispatcher;
		SubscriptionHandle handle;
	};

	std::vector<Entry> subscriptions;
};

#include "SubscriptionScope.inl"