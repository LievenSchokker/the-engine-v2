#pragma once


#include <cstdint>

struct ObjectHandle {
    uint32_t index;
    uint32_t generation;

    bool operator==(const ObjectHandle& other) const {
        return index == other.index && generation == other.generation;
    }

    bool operator!=(const ObjectHandle& other) const {
        return !(*this == other);
    }

    static ObjectHandle null() { return {0, 0}; }
    [[nodiscard]] bool isNull() const { return generation == 0; }
    [[nodiscard]] bool isValid() const { return generation != 0; }
};
