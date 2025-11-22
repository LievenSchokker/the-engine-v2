#pragma once


#include "IPhysicsWorld.h"
#include "Box2D/Box2DPhysicsWorld.h"
#include "Components/DynamicBody.h"


#include <memory>
#include <vector>

class PhysicsSystem {
public:
	PhysicsSystem();
	~PhysicsSystem() = default;

	void registerBody(DynamicBody* obj);
	void unregisterBody(DynamicBody* obj);

	void stepPhysics(float dt);
	void syncData();
	Box2DPhysicsWorld* GetWorld();

private:
	std::unique_ptr<Box2DPhysicsWorld> world;
	std::vector<DynamicBody*> dynamicBodies;
};