#pragma once


#include <memory>

template<typename T>
struct Slot {
	std::unique_ptr<T> object;
	uint32_t generation = 1;
};

