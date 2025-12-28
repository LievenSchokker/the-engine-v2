#pragma once


#include "Behaviour/Behaviour.h"
#include "Input/InputManager.h"
#include "Math/Vector2.h"


class TransformShooter : public Behaviour
{
public:
	TransformShooter()
		: inputManager(nullptr), activeScene(nullptr)
	{
	}

	~TransformShooter() override = default;

	void onAwake() override;
	void onEnable() override;
	void onStart() override;
	void update(double deltaTime, const GameWorld& gameWorld) override;
	void spawnGameObject();
	void onDisable() override;
private:
	float currentSpeed = 0.0f;
	float maxSpeed = 0.0f;

	InputManager* inputManager;

	std::vector<Transform*> listOfTransforms;
	Scene* activeScene;
};