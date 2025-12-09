//
// Created by samle on 08/12/2025.
//

#pragma once


#include "Math/Vector2.h"

struct BoundingBox
{
    BoundingBox(Vector2 minSize, Vector2 maxSize) : min(minSize), max(maxSize) {};
    ~BoundingBox() = default;

    Vector2 min;
    Vector2 max;

    Vector2 size() const;
    Vector2 center() const;
};
