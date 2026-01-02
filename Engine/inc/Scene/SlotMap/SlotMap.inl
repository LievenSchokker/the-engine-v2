#pragma once
#include "SlotMap.h"
#include "GameObject/ObjectHandle.h"

template<typename T>
ObjectHandle SlotMap<T>::add(std::unique_ptr<T> object) {
    uint32_t index;

    if (!freeList.empty()) {
        index = freeList.back();
        freeList.pop_back();
    } else {
        index = static_cast<uint32_t>(slots.size());
        slots.emplace_back();
    }

    slots[index].object = std::move(object);
    return {index, slots[index].generation};
}

template<typename T>
void SlotMap<T>::destroy(ObjectHandle handle) {
    if (!isValid(handle)) return;

    Slot& slot = slots[handle.index];
    slot.object.reset();
    slot.generation++;
    freeList.push_back(handle.index);
}

template<typename T>
T* SlotMap<T>::resolve(ObjectHandle handle) {
    if (!isValid(handle)) return nullptr;
    return slots[handle.index].object.get();
}

template<typename T>
const T* SlotMap<T>::resolve(ObjectHandle handle) const {
    if (!isValid(handle)) return nullptr;
    return slots[handle.index].object.get();
}

template<typename T>
std::unique_ptr<T> SlotMap<T>::extract(ObjectHandle handle) {
	if (!isValid(handle)) return nullptr;

	Slot& slot = slots[handle.index];
	auto result = std::move(slot.object);
	++slot.generation;
	freeList.push_back(handle.index);

	return result;
}

template<typename T>
bool SlotMap<T>::isValid(ObjectHandle handle) const {
    return handle.index < slots.size() &&
           slots[handle.index].generation == handle.generation &&
           slots[handle.index].object != nullptr;
}

template<typename T>
template<typename Func>
void SlotMap<T>::forEach(Func&& func) {
    for (auto& slot : slots) {
        if (slot.object) {
            func(*slot.object);
        }
    }
}

template<typename T>
template<typename Func>
void SlotMap<T>::forEach(Func&& func) const {
    for (const auto& slot : slots) {
        if (slot.object) {
            func(*slot.object);
        }
    }
}

template<typename T>
void SlotMap<T>::clear() {
    slots.clear();
    freeList.clear();
}

template<typename T>
const std::vector<typename SlotMap<T>::Slot>& SlotMap<T>::getSlots() const {
	return slots;
}