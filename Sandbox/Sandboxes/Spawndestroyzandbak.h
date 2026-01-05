#pragma once

#include "IZandbak.h"

/**
 * @class SpawnDestroyZandbak
 * @brief Test scene for verifying runtime GameObject spawning and destruction.
 *
 * Controls:
 *   L - Spawn new GameObjects with TestLifecycleBehaviour
 *   K - Destroy all spawned GameObjects
 *
 * This tests that:
 *   - GameObjects added at runtime have their behaviours properly initialized
 *   - All lifecycle methods (onAwake, onEnable, onStart, update) are called
 *   - Destruction properly calls onDisable and onDestroy
 */
class SpawnDestroyZandbak : public IZandbak
{
public:
	SpawnDestroyZandbak() = default;
	~SpawnDestroyZandbak() override = default;

	std::unique_ptr<Scene> getScene() override;
};