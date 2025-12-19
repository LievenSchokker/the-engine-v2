#include "Demo/PlayerMovement.h"
#include "Core/GameWorld.h"
#include "Input/InputManager.h"
#include "Input/KeyCode.h"
#include "Component/Transform.h"
#include "Component/ShapeRenderer.h"
#include "GameObject/GameObject.h"
#include "Networking/NetworkBuilder.h"

#include <iostream>

#include "Component/Camera.h"

PlayerMovement::PlayerMovement()
{
    authorityType = AuthorityType::ClientAuthority;
}

void PlayerMovement::onStart()
{
}

void PlayerMovement::onNetworkSpawn()
{
    if (hasAuthority())
    {
        auto camera = getGameObject()->addComponent<Camera>();
        camera->setZoom(0.2);
    }
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
}

void PlayerMovement::update(double deltaTime,const GameWorld& world)
{
	if (!hasAuthority())
	{
		return;
	}

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
	if (!gameWorld || !gameWorld->input) {
		return;
	}


    auto* input = gameWorld->input;
    if (input->isKeyDown(KeyCode::W) || input->isKeyDown(KeyCode::UP_ARROW))
    {
        callCommand("MoveUp");
        applyMovement(0, -1);
    }
    if (input->isKeyDown(KeyCode::S) || input->isKeyDown(KeyCode::DOWN_ARROW))
    {
        callCommand("MoveDown");
        applyMovement(0, 1);
    }
    if (input->isKeyDown(KeyCode::A) || input->isKeyDown(KeyCode::LEFT_ARROW))
    {
        callCommand("MoveLeft");
        applyMovement(-1, 0);
    }
    if (input->isKeyDown(KeyCode::D) || input->isKeyDown(KeyCode::RIGHT_ARROW))
    {
        callCommand("MoveRight");
        applyMovement(1, 0);
    }
}

void PlayerMovement::applyMovement(const float dirX, const float dirY) const
{

	if (!isServer())
	{
		return;
	}

	Transform* transform = getGameObject()->getTransform();
    float dt = 1.0f / 60.0f;
    Vector2 pos = transform->getPosition();
    pos.x += dirX * moveSpeed * dt;
    pos.y += dirY * moveSpeed * dt;
    transform->setPosition(pos);

    ShapeRenderer* shapeRenderer = nullptr;
    if (getGameObject()->tryGetComponent(shapeRenderer))
    {
        float currentRadius = shapeRenderer->getRadius();
        currentRadius += 0.5f;
        if (currentRadius > 50.0f)
        {
            currentRadius = 10.0f;
        }
        shapeRenderer->setCircle(currentRadius);
    }
}
