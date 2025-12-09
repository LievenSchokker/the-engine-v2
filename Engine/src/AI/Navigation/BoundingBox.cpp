//
// Created by samle on 08/12/2025.
//
#include "AI/Navigation/BoundingBox.h"
#include "Math/Vector2.h"



Vector2 BoundingBox::size() const
{
    return Vector2{ max.x - min.x, max.y - min.y };
}


Vector2 BoundingBox::center() const
{
    return Vector2{ (min.x + max.x) / 2, (min.y + max.y) / 2 };
}
