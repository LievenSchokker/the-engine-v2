#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"
#include "Math/Vector2.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"
#include "Core/ApplicationClock.h"


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
    builder.command("MoveUp", [this](ReadArchive&)
    {
        applyMovement(0, -1);
    });
    builder.command("MoveDown", [this](ReadArchive&)
    {
        applyMovement(0, 1);
    });
    builder.command("MoveLeft", [this](ReadArchive&)
    {
        applyMovement(-1, 0);
    });
    builder.command("MoveRight", [this](ReadArchive&)
    {
        applyMovement(1, 0);
    });
    builder.command("Stop", [this](ReadArchive&)
    {
        applyMovement(0, 0);
    });
}

void PlayerMovement::update(double deltaTime,const GameWorld& world)
{
	(void)deltaTime;
	(void)world;

	if (!hasAuthority())
	{
		return;
	}

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
	if (!gameWorld || !gameWorld->input) {
		return;
	}


    auto* input = gameWorld->input;
	bool moved = false;
    if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
    {
        callCommand("MoveUp");
        applyMovement(0, -100);
		moved = true;
    }
    if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
    {
        callCommand("MoveDown");
        applyMovement(0, 100);
		moved = true;
    }
    if (input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW))
    {
        callCommand("MoveLeft");
        applyMovement(-100, 0);
		moved = true;
    }
    if (input->isKeyDown(KeyCode::D) || input->isKeyDown(KeyCode::RIGHT_ARROW))
    {
        callCommand("MoveRight");
        applyMovement(100, 0);
		moved = true;
    }

	if (!moved)
	{
		callCommand("Stop");
		applyMovement(0, 0);
    }
}

void PlayerMovement::applyMovement(const float dirX, const float dirY) const
{
	if (!isServer())
	{
		return;
	}

	Vector2 direction{dirX, dirY};
	if (direction != Vector2::zero())
	{
		direction.normalize();
	}

	if (rigidBody != nullptr && gameWorld != nullptr &&
		gameWorld->physics != nullptr)
	{
		Vector2 desiredVelocity = Vector2::zero();
		if (direction != Vector2::zero())
		{
			desiredVelocity = direction * moveSpeed;
		}

		const Vector2 currentVelocity =
			gameWorld->physics->getLinearVelocity(rigidBody);
		float deltaTime = 1.0f / 60.0f;
		if (gameWorld->clock != nullptr)
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
	if (!transform)
	{
		return;
	}

    float dt = 1.0f / 60.0f;
    Vector2 pos = transform->getPosition();
    pos.x += direction.x * moveSpeed * dt;
    pos.y += direction.y * moveSpeed * dt;
    transform->setPosition(pos);
}
