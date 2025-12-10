#pragma once

#include <cstdint>

enum class ComponentType : uint32_t
{
	Unknown = 0,
	Transform,
	ShapeRenderer,
	TileMap,
	Grid,
};