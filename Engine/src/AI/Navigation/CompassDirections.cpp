//
// Created by samle on 10/12/2025.
//
#include "AI/Navigation/CompassDirections.h"
#include "Math/Vector2.h"

const std::vector<Vector2> CompassDirections::directions = {
    Vector2::up(),
    Vector2(1.0f, 1.0f),
    Vector2::right(),
    Vector2(1.0f, -1.0f),
    Vector2::down(),
    Vector2(-1.0f, -1.0f),
    Vector2::left(),
    Vector2(-1.0f, 1.0f),
};


const std::vector<Vector2> &CompassDirections::getDirections()
{
    return directions;
}

const std::vector<Vector2> &CompassDirections::getDirectionsNormalised()
{
     std::vector<Vector2> result;
     result.reserve(directions.size());

    for (const Vector2 &dir : CompassDirections::getDirections())
    {
        result.push_back(dir.normalised());
    }

    return result;
}
