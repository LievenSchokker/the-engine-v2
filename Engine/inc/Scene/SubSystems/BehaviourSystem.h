#pragma once

#include "Core/GameWorld.h"

#include <vector>

class Scene;
class Behaviour;

/**
 * @brief Manages behaviour lifecycle following Unity-style execution order.
 *
 * Ensures deterministic initialization (awake → enable → start) so behaviours
 * can safely reference each other after awake completes.
 */
class BehaviourSystem
{
public:
    BehaviourSystem() = default;
    ~BehaviourSystem() = default;
    BehaviourSystem(const BehaviourSystem&) = delete;
    BehaviourSystem& operator=(const BehaviourSystem&) = delete;

    /**
     * @brief Runs full lifecycle for all behaviours in scene.
     * Called once when scene first loads.
     */
    void initialiseScene(Scene& scene, GameWorld& world);

    /**
     * @brief Ticks enabled behaviours. Respects pause state via shouldRunWhenPaused().
     * @param deltaTime Frame time in seconds.
     */
    void update(Scene& scene, double deltaTime, const GameWorld& world);

	void fixedUpdate(Scene& scene, double deltaTime, const GameWorld& world);
    /**
     * @brief Calls awake() on behaviours that haven't yet awoken.
     * Runs before enable/start to allow one-time setup without dependencies on other behaviours.
     */
    void awakeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world);

    /**
     * @brief Calls onEnable() for behaviours whose GameObject is active.
     * Split from awake so behaviours can react to being toggled at runtime.
     */
    void enableBehaviours(const std::vector<Behaviour*>& behaviours);

    /**
     * @brief Calls start() once per behaviour after first enable.
     * Deferred from awake so other behaviours are guaranteed initialized.
     */
    void startBehaviours(const std::vector<Behaviour*>& behaviours);

    /**
     * @brief Calls onDisable() for cleanup when behaviours are turned off.
     */
    void disableBehaviours(const std::vector<Behaviour*>& behaviours);

    /**
     * @brief Lifecycle for behaviours added after scene init (e.g., spawned objects).
     */
    void initialiseRuntimeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world);
};