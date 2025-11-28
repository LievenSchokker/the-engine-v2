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
	void onStart() override;
	void onNetworkSpawn() override;
	void registerNetworkMethods(NetworkBuilder& builder) override;
	void update() override;

private:
	void handleInput();
	void applyMovement(float dirX, float dirY);

	float moveSpeed = 200.0f;
};