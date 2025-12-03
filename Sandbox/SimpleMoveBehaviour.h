//
// Created by samle on 26/11/2025.
//



#pragma once


#include "../Engine/inc/Input/InputManager.h"
#include "../Engine/inc/Behaviour/Behaviour.h"
#include "Math/Vector2.h"


class SimpleMoveBehaviour : public Behaviour
{
    public:
        SimpleMoveBehaviour():inputManager(nullptr), currentDirection(Vector2::zero()) {}
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
        float maxSpeed = 10.0f;
        float speedMultiplier = 1.5f;
        Vector2 currentDirection;

        InputManager* inputManager;
};
