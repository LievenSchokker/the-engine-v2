
#pragma once


struct Vector2;

#include <vector>


class CardinalDirections
{
    public:
        static const std::vector<Vector2>& getDirections();

    private:
        static const std::vector<Vector2> directions;
};