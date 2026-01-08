#pragma once

#include "Behaviour/NetworkBehaviour.h"

class GameWorld;
class RigidBody;

class PlayerMovement final : public NetworkBehaviour,
							 RegistrationBase<PlayerMovement>
{
public:
	PlayerMovement();

	static constexpr const char* name() { return "PlayerMovement"; }
	const char* getName() const override { return name(); }

	void onStart() override;
	void onNetworkSpawn() override;
	void registerNetworkMethods(NetworkBuilder& builder) override;
	void update(double deltaTime, const GameWorld& world) override;
	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

private:
	void handleInput();
	void applyMovement(float dirX, float dirY);

	RigidBody* rigidBody = nullptr;
	float moveSpeed = 300.0f;
	float acceleration = 12.0f;
	float braking = 18.0f;
};
