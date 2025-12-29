#pragma once


#include <cstdint>

/**
* @brief Handle for managing event subscriptions
 *
 * Keep this handle to unsubscribe later. Invalid handles are safe to use
 * (unsubscribe will simply do nothing).
 */
class SubscriptionHandle
{
public:
	SubscriptionHandle(uint64_t id, uint32_t typeId);

	[[nodiscard]] bool isValid() const;
	void invalidate();
	uint64_t id;
	uint32_t typeId;
};