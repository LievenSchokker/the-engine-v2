#pragma once

#include "Slot.h"

#include <memory>
#include <vector>

struct ObjectHandle;

/**
 * @brief Generational index container for stable O(1) handle-based access.
 *
 * Avoids ABA problem where a stale handle could resolve to a different object
 * that reused the same slot. Generation counter invalidates old handles on removal.
 *
 * @tparam T Stored type. Uses unique_ptr to support polymorphism and stable heap addresses.
 */
template<typename T>
class SlotMap {
public:
	/**
	 * @brief Inserts object, reusing freed slots when available.
	 * @return Handle valid until destroy() is called on it.
	 */
	ObjectHandle add(std::unique_ptr<T> object);

	/**
	 * @brief Releases object and increments generation to invalidate existing handles.
	 */
	void destroy(ObjectHandle handle);

	/**
	 * @brief O(1) lookup. Returns nullptr for stale or invalid handles.
	 */
	T* resolve(ObjectHandle handle);
	const T* resolve(ObjectHandle handle) const;

	/**
	 * @brief Removes and returns ownership. For transferring objects between containers.
	 */
	std::unique_ptr<T> extract(ObjectHandle handle);

	/**
	 * @brief Checks index bounds, generation match, and non-null object.
	 */
	[[nodiscard]] bool isValid(ObjectHandle handle) const;

	/**
	 * @brief Iterates only occupied slots. Tolerates removal during iteration
	 * only if not removing current element.
	 */
	template<typename Func>
	void forEach(Func&& func);

	template<typename Func>
	void forEach(Func&& func) const;

	void clear();

	/**
	 * @brief Direct slot access for serialization or debugging.
	 */
	const std::vector<Slot<T>>& getSlots() const;

private:
	std::vector<Slot<T>> slots;
	std::vector<uint32_t> freeList; 
};

#include "SlotMap.inl"