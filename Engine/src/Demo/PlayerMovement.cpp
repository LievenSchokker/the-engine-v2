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
    // Client → Server: movement input
    builder.command("Move", [this](ReadArchive& ar) {
        float dirX, dirY;
        ar.process(dirX);
        ar.process(dirY);
        serverMove(dirX, dirY);
    });

    // Server → Clients: position sync
    builder.clientRpc("UpdatePos", [this](ReadArchive& ar) {
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

    // Normalize diagonal movement
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f)
    {
        direction.x /= length;
        direction.y /= length;

        // For now, move locally (testing without network)
        float dt = 1.0f / 60.0f; // TODO: get actual delta time
        Vector2 pos = transform->getPosition();
        pos.x += direction.x * moveSpeed * dt;
        pos.y += direction.y * moveSpeed * dt;
        transform->setPosition(pos);

        // When networking is ready, send command instead:
        // callCommand("Move", direction.x, direction.y);
    }
}

void PlayerMovement::serverMove(float dirX, float dirY)
{
    // Server-side: validate and apply movement
    if (!isServer()) return;

    Transform* transform = getGameObject()->getTransform();
    if (!transform) return;

    // Clamp direction to prevent cheating
    float length = std::sqrt(dirX * dirX + dirY * dirY);
    if (length > 1.0f)
    {
        dirX /= length;
        dirY /= length;
    }

    float dt = 1.0f / 60.0f;
    Vector2 pos = transform->getPosition();
    pos.x += dirX * moveSpeed * dt;
    pos.y += dirY * moveSpeed * dt;
    transform->setPosition(pos);

    // Broadcast new position to all clients
    callRpc("UpdatePos", pos.x, pos.y);
}

void PlayerMovement::clientUpdatePosition(float x, float y)
{
    // Client-side: apply position from server
    if (!isClient()) return;

    // Don't override if we have authority (we're the one moving)
    if (hasAuthority()) return;

    Transform* transform = getGameObject()->getTransform();
    if (transform)
    {
        transform->setPosition({x, y});
    }
}