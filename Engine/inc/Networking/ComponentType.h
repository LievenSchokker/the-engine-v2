#pragma once

#include <cstdint>

enum class ComponentType : uint32_t
{
	Unknown = 0,
	Transform,        // 1
	ShapeRenderer,    // 2
	TileMap,          // 3
	Grid,             // 4
	NetworkIdentity,  // 5
	PlayerMovement,   // 6
};