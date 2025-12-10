//
// Created by samle on 08/12/2025.
//


#include "AI/Navigation/NavigationObstacle.h"
#include "AI/Navigation/BoundingBox.h"
#include "Component/Transform.h"
#include "Math/Vector2.h"
#include "Physics/Components/Collider.h"

NavigationObstacle::NavigationObstacle() : useManualBoundingBox(false), boundingBoxOffset(Vector2::zero()), manualBounds({Vector2::zero(), Vector2::zero()})
{

}


BoundingBox NavigationObstacle::getBounds()
{
    if (useManualBoundingBox)
    {
        Vector2 position = getTransform()->getPosition();
        return BoundingBox {
            position + manualBounds.min + boundingBoxOffset,
            position + manualBounds.max + boundingBoxOffset
        };
    }

    if (tryGetComponent<Collider>(collider))
    {
            /// TODO: Implement collider.getSize or something to retrieve its size and make a bounding box from it.
    }

    /// Default:
    return BoundingBox{Vector2::zero(), Vector2::zero() };
}

void NavigationObstacle::useManualBounds(Vector2 size)
{
    useManualBoundingBox = true;

    manualBounds.min = Vector2{-size.x / 2, -size.y / 2};
    manualBounds.max = Vector2{ size.x / 2,  size.y / 2};
}

void NavigationObstacle::setBoundsOffset(Vector2 offset)
{
    boundingBoxOffset = offset;
}


