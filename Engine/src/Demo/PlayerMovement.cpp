#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"

void PlayerMovement::onStart()
{
    if (getTransform())
    {
        lastPosition = getTransform()->getPosition();
    }
}

void PlayerMovement::onNetworkSpawn()
{
}

void PlayerMovement::registerNetworkMethods(NetworkBuilder& builder)
{
    builder.command("Move", [this](ReadArchive& ar) {
    });

}

void PlayerMovement::update()
{
    auto* world = getWorld();
    if (!world) return;

    if (world->isClient())
    {
        handleInput();
    }
}

void PlayerMovement::handleInput()
{
    auto* world = getWorld();
    if (!world || !world->input) return;

    if (!hasAuthority()) return;

    auto* input = world->input;
    Transform* transform = getGameObject()->getTransform();
    if (!transform) return;

    Vector2 direction{0, 0};

    if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
        direction.y -= 1.0f;
    if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
        direction.y += 1.0f;
    if (input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW))
        direction.x -= 1.0f;
    if (input->isKeyDown(KeyCode::D) || input->isKeyDown(KeyCode::RIGHT_ARROW))
        direction.x += 1.0f;

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.0f)
    {
        direction.x /= length;
        direction.y /= length;

        float dt = 1.0f / 60.0f;
        Vector2 pos = transform->getPosition();
        pos.x += direction.x * moveSpeed * dt;
        pos.y += direction.y * moveSpeed * dt;
        transform->setPosition(pos);

        callCommand("Move");
    }
}

void PlayerMovement::serverMove()
{
    Transform* transform = getGameObject()->getTransform();
    if (!transform) return;

    Vector2 pos = transform->getPosition();
}

void PlayerMovement::clientUpdatePosition(float x, float y)
{
    if (!isClient()) return;

    if (hasAuthority()) return;

    Transform* transform = getGameObject()->getTransform();
    if (transform)
    {
        transform->setPosition({x, y});
    }
}