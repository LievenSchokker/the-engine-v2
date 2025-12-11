//
// Created by samle on 11/12/2025.
//

#pragma once
#include <vector>

#include "Math/Vector2.h"


class IGraph
{
    public:
        virtual ~IGraph() = default;

        virtual std::vector<Vector2> getNeighbours(Vector2 node) const = 0;
        virtual float getCost(Vector2 from, Vector2 to) const = 0;
        virtual bool isValid(Vector2 node) const = 0;
};
