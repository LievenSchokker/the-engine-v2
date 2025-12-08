//
// Created by samle on 08/12/2025.
//

#pragma once


struct Vector2;


struct BoundingBox
{
    Vector2 min;
    Vector2 max;

    Vector2 size() const;
    Vector2 center() const;
};
