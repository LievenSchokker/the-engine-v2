#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"

#include <iostream>

void PlayerMovement::onStart()
{
}

void PlayerMovement::onNetworkSpawn()
{
    std::cout << "[PlayerMovement] onNetworkSpawn - isServer: " << isServer()
              << " isClient: " << isClient()
              << " hasAuthority: " << hasAuthority() << std::endl;
}

void PlayerMovement::registerNetworkMethods(NetworkBuilder& builder)
{
    std::cout << "[PlayerMovement] Registering network methods" << std::endl;

    builder.command("MoveUp", [this](ReadArchive&) {
        applyMovement(0, -1);
    });
    builder.command("MoveDown", [this](ReadArchive&) {
        applyMovement(0, 1);
    });
    builder.command("MoveLeft", [this](ReadArchive&) {
        applyMovement(-1, 0);
    });
    builder.command("MoveRight", [this](ReadArchive&) {
        applyMovement(1, 0);
    });
}

void PlayerMovement::update(float deltaTime, GameWorld* world)
{
    if (hasAuthority())
    {
        handleInput();
    }
}
void PlayerMovement::handleInput()
{
    auto* world = getWorld();
    if (!world || !world->input) return;

    auto* input = world->input;

    if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
    {
        callCommand("MoveUp");
    	applyMovement(0, 1);
    }
    if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
    {
        callCommand("MoveDown");
    	applyMovement(0, -1);
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

void PlayerMovement::applyMovement(float dirX, float dirY)
{
    if (!isServer())
    {
        return;
    }

    Transform* transform = getGameObject()->getTransform();

    float dt = 1.0f / 60.0f;
    Vector2 pos = transform->getPosition();
    pos.x += dirX * moveSpeed * dt;
    pos.y += dirY * moveSpeed * dt;
    transform->setPosition(pos);
}