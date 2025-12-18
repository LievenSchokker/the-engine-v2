#pragma once


#include "Behaviour/NetworkBehaviour.h"
#include "Math/Vector2.h"

class GameWorld;

/**
 * @brief Simple networked player movement.
 *
 * Client: Reads input, sends movement commands to server
 * Server: Validates and applies movement, broadcasts to clients
 */
class PlayerMovement final: public NetworkBehaviour,
                            RegistrationBase<PlayerMovement>
{
public:
	PlayerMovement();

	static constexpr const char* name()
	{
		return "PlayerMovement";
	}
	void onStart() override;
	void onNetworkSpawn() override;
	void registerNetworkMethods(NetworkBuilder& builder) override;
	void update(float deltaTime, GameWorld* world) override;
	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

private:
	void handleInput();
	void applyMovement(float dirX, float dirY) const;

	float moveSpeed = 200.0f;
};