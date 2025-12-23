#pragma once


#include "Core/GameWorld.h"

#include <vector>

class Scene;
class Behaviour;

class BehaviourSystem
{
public:
	BehaviourSystem() = default;
	~BehaviourSystem() = default;
	BehaviourSystem(const BehaviourSystem&) = delete;
	BehaviourSystem& operator=(const BehaviourSystem&) = delete;

	void initialiseScene(Scene& scene, GameWorld& world);
	void update(Scene& scene, double deltaTime, const GameWorld& world);

	void awakeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world);
	void enableBehaviours(const std::vector<Behaviour*>& behaviours);
	void startBehaviours(const std::vector<Behaviour*>& behaviours);
	void disableBehaviours(const std::vector<Behaviour*>& behaviours);
	void initialiseRuntimeBehaviours(const std::vector<Behaviour*>& behaviours, GameWorld& world);
};