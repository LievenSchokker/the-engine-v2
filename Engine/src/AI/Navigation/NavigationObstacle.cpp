//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/BoundingBox.h"
#include "Math/Vector2.h"
#include "Physics/Components/Collider.h"


BoundingBox NavigationObstacle::getBounds()
{
    if (useManualBoundingBox)
    {
        return BoundingBox {manualBounds.min + boundingBoxOffset.first, manualBounds.max + boundingBoxOffset.second };
    }

    if (tryGetComponent<Collider>(collider))
    {
            /// TODO: Implement collider.getSize or something to retrieve its size and make a bounding box from it.
    }

    /// Default:
    return BoundingBox{Vector2::zero(), Vector2::zero() };
}

void NavigationObstacle::useManualBounds(BoundingBox bounds)
{
    useManualBoundingBox = true;
    manualBounds = bounds;
}

void NavigationObstacle::setBoundsOffset(std::pair<Vector2, Vector2> boundsOffset)
{
    boundingBoxOffset = boundsOffset;
}


