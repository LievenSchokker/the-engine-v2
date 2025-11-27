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
    // Network-specific init if needed
}

void PlayerMovement::registerNetworkMethods(NetworkBuilder& builder)
{
    builder.command("Move", [this](CerealReadArchive& ar) {
        float dirX, dirY;
        ar.process(dirX);
        ar.process(dirY);
        serverMove(dirX, dirY);
    });

    builder.clientRpc("UpdatePos", [this](CerealReadArchive& ar) {
        float x, y;
        ar.process(x);
        ar.process(y);
        clientUpdatePosition(x, y);
    });
}

void PlayerMovement::update()
{
    auto* world = getWorld();
    if (!world) return;

    // For now, handle movement locally for testing
    // Once networking is wired up, only the authoritative side moves
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

        // Move locally
        float dt = 1.0f / 60.0f;
        Vector2 pos = transform->getPosition();
        pos.x += direction.x * moveSpeed * dt;
        pos.y += direction.y * moveSpeed * dt;
        transform->setPosition(pos);

        // Send to server (all clients will send for now)
        callCommand("Move", direction.x, direction.y);
    }
}

void PlayerMovement::serverMove(float dirX, float dirY)
{
    // For now, since we can't pass params, just sync current position
    Transform* transform = getGameObject()->getTransform();
    if (!transform) return;

    Vector2 pos = transform->getPosition();

    // Broadcast - but we can't pass pos.x, pos.y without payload support
    callRpc("UpdatePos");
}

void PlayerMovement::clientUpdatePosition(float x, float y)
{
    if (!isClient()) return;

    // Skip if we're the owner (we already moved locally)
    if (hasAuthority()) return;

    Transform* transform = getGameObject()->getTransform();
    if (transform)
    {
        transform->setPosition({x, y});
    }
}