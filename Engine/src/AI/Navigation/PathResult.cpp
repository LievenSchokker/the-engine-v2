//
// Created by samle on 08/12/2025.
//

#include "../../../inc/AI/Navigation/PathResult.h"
#include "Math/Vector2.h"

const std::vector<Vector2> &PathResult::getPath() const
{
    return path;
}

int PathResult::getPathSize() const
{
    if (!isValid())
        return 0;

    return path.size();
}


float PathResult::getPathLength() const
{
    if (path.size() < 2)
        return 0.0f;

    float totalLength = 0.0f;

    for (size_t i = 1; i < path.size(); ++i)
    {
        const Vector2& prev = path[i - 1];
        const Vector2& curr = path[i];

        totalLength += Vector2::distance(curr, prev);
    }

    return totalLength;
}

bool PathResult::isValid() const
{
    return path.size() > 0 && getPathLength() > 0;
}
