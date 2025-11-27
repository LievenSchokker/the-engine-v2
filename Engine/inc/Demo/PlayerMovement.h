#pragma once

#include "Behaviour/NetworkBehaviour.h"
#include "GameObject/Vector2.h"

/**
 * @brief Simple networked player movement.
 *
 * Client: Reads input, sends movement commands to server
 * Server: Validates and applies movement, broadcasts to clients
 */
class PlayerMovement : public NetworkBehaviour
{
public:
    PlayerMovement() = default;
    ~PlayerMovement() override = default;

    void onStart() override;
    void update() override;

    void onNetworkSpawn() override;
    void registerNetworkMethods(NetworkBuilder& builder) override;

    // Movement settings
    float moveSpeed = 200.0f;

private:
    // Called on server when client sends movement input
    void serverMove();

    // Called on all clients to sync position
    void clientUpdatePosition(float x, float y);

    // Local input handling
    void handleInput();

    Vector2 lastPosition{0, 0};
};