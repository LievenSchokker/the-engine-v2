#pragma once

#include "Events/Event.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

/**
 * @brief Handle for managing event subscriptions
 *
 * Keep this handle to unsubscribe later. Invalid handles are safe to use
 * (unsubscribe will simply do nothing).
 */
class SubscriptionHandle
{
public:
	SubscriptionHandle() : id(0), typeId(0)
	{
	}

	bool isValid() const
	{
		return id != 0;
	}

	void invalidate()
	{
		id = 0;
	}

private:
	friend class EventDispatcher;

	SubscriptionHandle(uint64_t id, uint32_t typeId)
		: id(id), typeId(typeId)
	{
	}

	uint64_t id;
	uint32_t typeId;
};

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

	// Non-copyable
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;

	/**
	 * @brief Subscribe to an event type
	 * @tparam T Event type to subscribe to
	 * @param callback Function to call when event is dispatched
	 * @return Handle for unsubscribing
	 */
	template <typename T>
	SubscriptionHandle subscribe(std::function<void(const T&)> callback)
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
		return SubscriptionHandle(id, typeId);
	}

	/**
	 * @brief Subscribe with a lambda (convenience overload)
	 */
	template <typename T, typename F>
	SubscriptionHandle subscribe(F&& callback)
	{
		return subscribe<T>(
			std::function<void(const T&)>(std::forward<F>(callback)));
	}

	/**
	 * @brief Unsubscribe using a handle
	 */
	void unsubscribe(SubscriptionHandle& handle);

	/**
	 * @brief Unsubscribe all listeners for a specific event type
	 */
	template <typename T>
	void unsubscribeAll()
	{
		uint32_t typeId = T::staticTypeId();
		listeners.erase(typeId);
	}

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
	void dispatch(T& event)
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

	/**
	 * @brief Dispatch an event (const overload, creates copy)
	 */
	template <typename T>
	void dispatch(const T& event)
	{
		T copy = event;
		dispatch(copy);
	}

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