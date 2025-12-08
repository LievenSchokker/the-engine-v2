//
// Created by samle on 08/12/2025.
//


#include "NavigationObstacle.h"
#include "Math/Vector2.h"


BoundingBox NavigationObstacle::getBounds()
{
    if (useManualBoundingBox)
    {

    }
    else
    {
        if (tryGetComponent<Collider>(collider))
        {


        }
    }
}

void NavigationObstacle::useManualBounds(BoundingBox bounds)
{
    useManualBoundingBox = true;
    /// etc
}

void NavigationObstacle::setBoundsOffset(Vector2 boundsOffset)
{
    offset = boundsOffset;
}


