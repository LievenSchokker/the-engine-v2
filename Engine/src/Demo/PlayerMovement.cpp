#include "Demo/PlayerMovement.h"

#include "Component/Transform.h"
#include "Core/ApplicationClock.h"
#include "Core/GameWorld.h"
#include "GameObject/GameObject.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Math/Vector2.h"
#include "Networking/NetworkBuilder.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"

PlayerMovement::PlayerMovement()
{
	authorityType = AuthorityType::ClientAuthority;
}

void PlayerMovement::onStart()
{
	rigidBody = getComponent<RigidBody>();
}

void PlayerMovement::onNetworkSpawn()
{
}

void PlayerMovement::registerNetworkMethods(NetworkBuilder& builder)
{
	builder.command("Move", &PlayerMovement::applyMovement);
}

void PlayerMovement::update(double deltaTime, const GameWorld& world)
{
	(void)deltaTime;
	(void)world;

	if ( !hasAuthority() ) return;

	handleInput();
}

void PlayerMovement::serialize(WriteArchive& archive) const
{
	archive.process(const_cast<float&>(moveSpeed));
	archive.process(const_cast<float&>(acceleration));
	archive.process(const_cast<float&>(braking));
}

void PlayerMovement::deserialize(ReadArchive& archive)
{
	archive.process(moveSpeed);
	archive.process(acceleration);
	archive.process(braking);
}

void PlayerMovement::handleInput()
{
	if ( !gameWorld || !gameWorld->input ) return;

	auto* input = gameWorld->input;
	float dirX = 0.0f;
	float dirY = 0.0f;

	if ( input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW) )
		dirY -= 1.0f;
	if ( input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW) )
		dirY += 1.0f;
	if ( input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW) )
		dirX -= 1.0f;
	if ( input->isKeyDown(KeyCode::D) ||
		 input->isKeyDown(KeyCode::RIGHT_ARROW) )
		dirX += 1.0f;

	if ( dirX != 0.0f || dirY != 0.0f )
	{
		callCommand("Move", dirX, dirY);
		applyMovement(dirX, dirY);
	}
}

void PlayerMovement::applyMovement(float dirX, float dirY)
{
	Vector2 direction{dirX, dirY};
	if ( direction != Vector2::zero() )
	{
		direction.normalize();
	}

	if ( rigidBody != nullptr && gameWorld != nullptr &&
		 gameWorld->physics != nullptr )
	{
		Vector2 desiredVelocity = Vector2::zero();
		if ( direction != Vector2::zero() )
		{
			desiredVelocity = direction * moveSpeed;
		}

		const Vector2 currentVelocity =
			gameWorld->physics->getLinearVelocity(rigidBody);
		float deltaTime = 1.0f / 60.0f;
		if ( gameWorld->clock != nullptr )
		{
			deltaTime = static_cast<float>(gameWorld->clock->getDeltaTime());
		}
		const float rate =
			(direction != Vector2::zero()) ? acceleration : braking;
		const Vector2 newVelocity =
			Vector2::lerp(currentVelocity, desiredVelocity, rate * deltaTime);
		gameWorld->physics->setLinearVelocity(rigidBody, newVelocity);
		return;
	}

	Transform* transform = getGameObject()->getTransform();
	if ( !transform )
	{
		return;
	}

	float dt = 1.0f / 60.0f;
	Vector2 pos = transform->getPosition();
	pos.x += direction.x * moveSpeed * dt;
	pos.y += direction.y * moveSpeed * dt;
	transform->setPosition(pos);
}