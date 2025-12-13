#pragma once


#include "EventDispatcher/EventDispatcher.h"

#include <queue>
#include <mutex>
#include <functional>

/**
 * @brief Thread-safe event queue for deferred event processing
 *
 * Events are queued and processed later in a controlled manner,
 * typically once per frame. This decouples event producers from
 * consumers and allows for thread-safe event pushing.
 */
class EventQueue
{
public:
	EventQueue() = default;
	~EventQueue() = default;

	/**
	 * @brief Push an event onto the queue
	 * @tparam T Event type
	 * @param event Event to queue
	 */
	template <typename T>
	void push(T event)
	{
		static_assert(std::is_base_of_v<EventBase, T>,
		              "T must derive from Event<T>");

		pendingEvents.push(
			[event = std::move(event)](EventDispatcher& d) mutable
			{
				d.dispatch(event);
			});
	}

	/**
	 * @brief Construct and push an event in-place
	 * @tparam T Event type
	 * @tparam Args Constructor argument types
	 * @param args Constructor arguments
	 */
	template <typename T, typename... Args>
	void emplace(Args&&... args)
	{
		push(T(std::forward<Args>(args)...));
	}

	/**
	 * @brief Process all queued events
	 * @param dispatcher Dispatcher to use for dispatching events
	 *
	 * Call this once per frame from your main thread.
	 */
	void processAll(EventDispatcher& dispatcher);

	/**
	 * @brief Clear all pending events without processing
	 */
	void clear();

	/**
	 * @brief Check if queue is empty
	 */
	bool empty() const;

	/**
	 * @brief Get number of pending events
	 */
	std::size_t size() const;

private:
	using EventAction = std::function<void(EventDispatcher&)>;

	std::queue<EventAction> pendingEvents;
	mutable std::mutex mutex;
};