//
// Created by Lieven Schokker on 06/01/2026.
//

#include "PhysicsMovement.h"

#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "Component/ShapeRenderer.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"
#include "Physics/Components/RigidBody.h"

#include <iostream>

PhysicsMovement::PhysicsMovement()
{
    authorityType = SpawnOnType::ClientSpawn;
}

void PhysicsMovement::onStart()
{
}

void PhysicsMovement::onNetworkSpawn()
{
}

void PhysicsMovement::registerNetworkMethods(NetworkBuilder& builder)
{
    builder.command("MoveUp", [this](ReadArchive&)
    {
        applyMovement(0, -1);
    });
    builder.command("MoveDown", [this](ReadArchive&)
    {
        applyMovement(0, 1);
    });
    builder.command("MoveLeft", [this](ReadArchive&)
    {
        applyMovement(-1, 0);
    });
    builder.command("MoveRight", [this](ReadArchive&)
    {
        applyMovement(1, 0);
    });
}

void PhysicsMovement::update(double deltaTime, const GameWorld& world)
{
    if (!hasAuthority())
    {
        return;
    }

    handleInput();
}

void PhysicsMovement::serialize(WriteArchive& archive) const
{
    archive.process(const_cast<float&>(moveSpeed));
}

void PhysicsMovement::deserialize(ReadArchive& archive)
{
    archive.process(moveSpeed);
}

void PhysicsMovement::handleInput()
{
    if (!gameWorld || !gameWorld->input)
    {
        return;
    }

    auto* input = gameWorld->input;
    if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
    {
        callCommand("MoveUp");
        applyMovement(0, -1);
    }
    if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
    {
        callCommand("MoveDown");
        applyMovement(0, 1);
    }
    if (input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW))
    {
        callCommand("MoveLeft");
        applyMovement(-1, 0);
    }
    if (input->isKeyDown(KeyCode::D) || input->isKeyDown(KeyCode::RIGHT_ARROW))
    {
        callCommand("MoveRight");
        applyMovement(1, 0);
    }
}

void PhysicsMovement::applyMovement(const float dirX, const float dirY) const
{
    GameObject* go = getGameObject();
    if (!go) return;

    auto* rb = go->getComponent<RigidBody>();

    if (rb && gameWorld && gameWorld->physics)
    {
        Vector2 force(dirX * moveSpeed, dirY * moveSpeed);
        gameWorld->physics->applyForce(rb, force * 10000);
    }
    else
    {
        Transform* transform = go->getTransform();
        float dt = 1.0f / 60.0f;
        Vector2 pos = transform->getPosition();
        pos.x += dirX * moveSpeed * dt;
        pos.y += dirY * moveSpeed * dt;
        transform->setPosition(pos);
    }
}