#pragma once


#include <memory>
#include <vector>

struct ObjectHandle;

template<typename T>
class SlotMap {
public:
	struct Slot {
		std::unique_ptr<T> object;
		uint32_t generation = 1;
	};

	ObjectHandle add(std::unique_ptr<T> object);
	void destroy(ObjectHandle handle);
	T* resolve(ObjectHandle handle);
	const T* resolve(ObjectHandle handle) const;
	std::unique_ptr<T> extract(ObjectHandle handle);
	[[nodiscard]] bool isValid(ObjectHandle handle) const;

	template<typename Func>
	void forEach(Func&& func);

	template<typename Func>
	void forEach(Func&& func) const;

	void clear();
	const std::vector<Slot>& getSlots() const;

private:
	std::vector<Slot> slots;
	std::vector<uint32_t> freeList;
};

#include "SlotMap.inl"