#pragma once


#include <memory>


class GameObject;

struct Slot {
    std::unique_ptr<GameObject> object;
    uint32_t generation = 1;
};
