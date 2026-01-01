#pragma once


#include <cstdint>

struct GameObjectHandle {
    uint32_t index;
    uint32_t generation;

    bool operator==(const GameObjectHandle& other) const {
        return index == other.index && generation == other.generation;
    }

    bool operator!=(const GameObjectHandle& other) const {
        return !(*this == other);
    }

    static GameObjectHandle null() { return {0, 0}; }
    bool isNull() const { return generation == 0; }
    bool isValid() const { return generation != 0; }
};
