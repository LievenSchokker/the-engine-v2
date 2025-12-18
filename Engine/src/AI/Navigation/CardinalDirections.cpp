
#include "AI/Navigation/CardinalDirections.h"

#include "Math/Vector2.h"


const std::vector<Vector2> CardinalDirections::directions =
{
        Vector2::up(),
        Vector2::right(),
        Vector2::down(),
        Vector2::left()
};

const std::vector<Vector2>& CardinalDirections::getDirections()
{
    return directions;
}
