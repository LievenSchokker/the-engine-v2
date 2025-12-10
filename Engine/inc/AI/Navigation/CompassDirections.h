
#pragma once


struct Vector2;

#include <vector>


class CompassDirections
{
    public:
        static const std::vector<Vector2>& getDirections();
        static const std::vector<Vector2>& getDirectionsNormalised();

    private:
        static const std::vector<Vector2> directions;
};
