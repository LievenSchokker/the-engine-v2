#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"

PlayerMovement::PlayerMovement()
{
	authorityType = AuthorityType::ClientAuthority;
}

void PlayerMovement::onNetworkSpawn()
{
}

void PlayerMovement::registerNetworkMethods(NetworkBuilder& builder)
{
	// Clean! Just point to the method - args are auto-deserialized
	builder.command("Move", &PlayerMovement::applyMovement);
}

void PlayerMovement::update(double deltaTime, const GameWorld& world)
{
	if (!hasAuthority())
		return;

	handleInput();
}

void PlayerMovement::serialize(WriteArchive& archive) const
{
	archive.process(const_cast<float&>(moveSpeed));
}

void PlayerMovement::deserialize(ReadArchive& archive)
{
	archive.process(moveSpeed);
}

void PlayerMovement::handleInput()
{
	if (!gameWorld || !gameWorld->input)
		return;

	auto* input = gameWorld->input;

	float dirX = 0.0f;
	float dirY = 0.0f;

	if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
		dirY -= 1.0f;
	if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
		dirY += 1.0f;
	if (input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW))
		dirX -= 1.0f;
	if (input->isKeyDown(KeyCode::D) || input->isKeyDown(KeyCode::RIGHT_ARROW))
		dirX += 1.0f;

	if (dirX != 0.0f || dirY != 0.0f)
	{
		callCommand("Move", dirX, dirY);
		applyMovement(dirX, dirY);
	}
}

void PlayerMovement::applyMovement(float dirX, float dirY)
{
	Transform* transform = getGameObject()->getTransform();
	float dt = 1.0f / 60.0f;

	Vector2 pos = transform->getPosition();
	pos.x += dirX * moveSpeed * dt;
	pos.y += dirY * moveSpeed * dt;
	transform->setPosition(pos);
}