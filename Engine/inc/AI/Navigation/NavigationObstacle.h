//
// Created by samle on 08/12/2025.
//

#pragma once


#include "AI/Navigation/BoundingBox.h"
#include "Component/BaseComponentTypes/Component.h"
#include "Math/Vector2.h"

class Collider;

/**
 * @brief This component can be put on GameObjects that have a Collider component to mark their surface area as unwalkable for navigation
 *
 * The @c BoundingBox of the object is calculated using its @c Collider component, which can be altered using the @c setBoundsOffset() method.
 */
class NavigationObstacle : public Component
{
    public:
        /**
         * @brief Default constructor attempts to use this GameObject's Collider component to set this obstacle's bounding box
         */
        NavigationObstacle();

        /**
         * @brief This constructor disregards this GameObject's Collider component, and uses the given size for its bounding box instead
         * @param manualSize desired size of the bounding box
         * @param offset Offset of the bounding box, relative to this GameObject's position
         */
        NavigationObstacle(Vector2 manualSize, Vector2 offset);

        /// Default destructor
        ~NavigationObstacle() override = default;

        /**
         * @brief retrieves the bounding box of this obstacle, used to mark its underlying cells on the @c NavigationGrid as unwalkable
         * @return The bounding box, computed by this obstacle's collider + offset, or the manually set size if @c useManualSize was called.
         */
        BoundingBox getBounds();

        /**
         * @brief Sets the offset which is used to compute the BoundingBox of this obstacle.
         *
         *  First Vector
         *
         * @param boundsOffset pair of Vector2;
         * First element is the offset to the min size of the bounds,
         * Second element is the offset to the max size of the bounds
         */
        void setBoundsOffset(Vector2 boundsOffset);

        /**
         * @brief allows obstacles to be carved by setting a predefined bounds instead of using the collider component to create it.
         *
         * Note; If using manual size, offset gets added on top of the predefined size.
         * @bounds Bounds to be used to mark the obstacle on the grid.
         */
        void useManualBounds(Vector2 size);

    private:
        Collider* collider;
        Vector2 boundingBoxOffset;
        bool useManualBoundingBox;
        BoundingBox manualBounds;
};
