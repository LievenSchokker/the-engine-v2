
#pragma once


struct Vector2;
#include <vector>


/**
 * @brief Utility class for getting all compass directions (N, NE, E, SE, S, SW, W, NW) contained in a std::vector
 *
 * Each direction is stored in a normalized 2D unit vector2
 */
class CompassDirections
{
    public:
        /**
        * @brief Returns the raw compass direction vectors.
        *
        * The returned vectors represent the eight compass directions using
        * axis-aligned and diagonal vectors:
        *  - Cardinal directions have a length of 1 (e.g. (0, 1), (1, 0))
        *  - Diagonal directions have a length greater than 1 (e.g. (1, 1))
        *
        * These vectors are suitable when the relative magnitude of the direction
        * is important (e.g. grid offsets or unnormalised movement).
        */
        static const std::vector<Vector2>& getDirections();

        /**
        * @brief Returns the compass directions as normalized unit vectors.
        *
        * All returned vectors are normalized to unit length (length = 1),
        * including diagonal directions. This ensures consistent magnitude
        * across all directions.
        *
        * This is useful for direction-only calculations such as movement,
        * steering, or angle-based logic where equal speed in all directions
        * is required.
        *
        * The normalized directions are computed once and cached for reuse.
        */
        static const std::vector<Vector2>& getDirectionsNormalised();

    private:
        static const std::vector<Vector2> directions;
};
