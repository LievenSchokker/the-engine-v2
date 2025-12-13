//
// Created by samle on 08/12/2025.
//

#pragma once


#include "Math/Vector2.h"

/**
 * Struct representing a bounding box, described by a min and max vector2.
 *
 * Used by the engine for navigation purposes.
 */
struct BoundingBox
{
    BoundingBox(Vector2 minSize, Vector2 maxSize) : min(minSize), max(maxSize) {};
    ~BoundingBox() = default;

    /**
     * @brief represents the total size of this bounding box.
     *
     * @return the difference between min and max in a new Vector2
     */
    [[nodiscard]] Vector2 size() const;

    /**
     * @brief Represents the center point inside this bounding box
     *
     * @return A vector2 representing the center of the box, calculated by adding min and max Vector2's and dividing them by 2
     */
    [[nodiscard]] Vector2 center() const;

    /**
     * @brief Minimum point of the bounding box, representing the lower-left point of the box.
     */
    Vector2 min;

    /**
     * @brief Maximum point of the bounding box, representing the top-right point of the box.
     */
    Vector2 max;
};
