#pragma once

#include "GameObject/GameObject.h"

#include <algorithm>
#include <limits>

/**
 * @brief Clamps a GameObject's layer value to fit within uint8_t range.
 * @param owner The GameObject whose layer should be clamped. Can be nullptr.
 * @return The clamped layer value as uint8_t, or 0 if owner is nullptr.
 */
inline uint8_t clampRenderLayer(const GameObject* owner)
{
	if ( owner == nullptr )
	{
		return 0;
	}

	const int maxLayer = static_cast<int>(std::numeric_limits<uint8_t>::max());
	const int clamped = std::clamp(owner->getLayer(), 0, maxLayer);
	return static_cast<uint8_t>(clamped);
}
