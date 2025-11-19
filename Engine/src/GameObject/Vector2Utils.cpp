#include "../../inc/GameObject/Vector2Utils.h"

#include <cmath>

namespace
{
constexpr double kEpsilon = 1e-6;
}

Vector2 Vector2Utils::sanitizeScale(Vector2 scale)
{
    if (std::abs(scale.x) < kEpsilon)
    {
        scale.x = 1.0;
    }

    if (std::abs(scale.y) < kEpsilon)
    {
        scale.y = 1.0;
    }

    return scale;
}

