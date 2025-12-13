
#pragma once


struct Vector2;
#include <vector>


/**
 * @brief Utility class for getting the Cardinal Directions (NESW) contained in a std::vector.
 *
 * Stores each direction as a normalized 2D unit Vector2:
 * Directions are stored in order:
 * - Up (0, 1)
 * - Right (1,0)
 * - Down (0, -1)
 * - Left (-1,0)
 */
class CardinalDirections
{
    public:
    static const std::vector<Vector2>& getDirections();

    private:
        static const std::vector<Vector2> directions;
};