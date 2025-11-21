#pragma once

#include "Vector2.h"

/**
 * @brief Utility functions for Vector2 operations.
 */
namespace Vector2Utils
{
/**
 * @brief Sanitizes the scale of a vector to prevent division by zero.
 *
 * If either component is near zero (within epsilon), it is set to 1.0.
 *
 * @param scale The vector to sanitize.
 * @return The sanitized vector.
 */
Vector2 sanitizeScale(Vector2 scale);
}  // namespace Vector2Utils
