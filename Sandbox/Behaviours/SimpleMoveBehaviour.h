//
// Created by samle on 26/11/2025.
//

#pragma once

#include "Behaviour/Behaviour.h"
#include "Input/InputManager.h"
#include "Math/Vector2.h"

#include <iostream>

class SimpleMoveBehaviour: public Behaviour
{
   public:
	SimpleMoveBehaviour()
		: inputManager(nullptr), currentDirection(Vector2::zero())
	{
	}

	~SimpleMoveBehaviour() override = default;

	void onAwake() override;
	void onEnable() override;
	void onStart() override;
	void update(double deltaTime, const GameWorld& gameWorld) override;
	void onDisable() override;
	void setMaxSpeed(float speed) { maxSpeed = speed; }

	void move();
	void pollInput();

   private:
	bool canMove = false;
	float currentSpeed = 0.0f;
	float maxSpeed = 0.0f;
	Vector2 currentDirection;
    std::unique_ptr<GameObject> clone;
	InputManager* inputManager;
};
