//
// Created by samle on 26/11/2025.
//

#pragma once
#include "Behaviour.h"
#include "Input/InputManager.h"
struct Vector2;

class SimpleMoveBehaviour : public Behaviour
{
    public:
        SimpleMoveBehaviour();
        ~SimpleMoveBehaviour() override = default;

        void onAwake() override;
        void onEnable() override;
        void onStart() override;
        void update() override;
        void onDisable() override;

        void move();
        void pollInput();

    private:
        bool canMove = false;
        float currentSpeed = 0.0f;
        float maxSpeed = 10f;
        float speedMultiplier = 1.5f;
        Vector2 currentDirection;

        InputManager* inputManager;
};
