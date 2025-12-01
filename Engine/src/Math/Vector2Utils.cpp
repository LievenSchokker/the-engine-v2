#include "../../inc/Math/Vector2Utils.h"

#include <cmath>

namespace
{
constexpr double kEpsilon = 1e-6;
}

Vector2 Vector2Utils::sanitizeScale(Vector2 scale)
{
	if ( std::abs(scale.X()) < kEpsilon ) {
		scale.setX(1.0);;
	}

	if ( std::abs(scale.Y()) < kEpsilon ) {
		scale.setY(1.0);;
	}

	return scale;
}
