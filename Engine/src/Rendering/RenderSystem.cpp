#include "Rendering/RenderSystem.h"

#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/UIElement/UIElement.h"
#include "Rendering/IRenderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Core/IEngineSystems.h"

#include <iostream>

RenderSystem::RenderSystem(std::unique_ptr<IRenderer> renderer)
	: renderer(std::move(renderer))
{
}

void RenderSystem::update(double deltaTime, const GameWorld& gameWorld)
{
	if (!renderer || !renderer->isOpen())
	{
		return;
	}

	queue.clearAll();
	collectCommands(*gameWorld.sceneManager->getActiveScene());
	queue.sortAll();
	renderer->beginFrame(clearColor);

	for (auto& command : queue.world().getCommands())
	{
		renderer->execute(command);
	}

	// renderer->submitUI(queue.ui().getCommands());s

	renderer->endFrame();
}

Phase RenderSystem::getUpdatePhase() const
{
	return Phase::Late;
}

void RenderSystem::collectCommands(Scene& scene)
{
	for (auto* component : scene.getAllComponentsOfType<RenderComponent>())
	{
		if (component == nullptr) continue;

		auto* gameObject = component->getGameObject();
		if (gameObject == nullptr || !gameObject->getIsActive()) continue;

		component->fillRenderQueue(queue);
	}

	for (auto* component :
	     scene.getAllComponentsOfType<UserInterfaceRenderComponent>())
	{
		if (component == nullptr) continue;

		auto* gameObject = component->getGameObject();
		if (gameObject == nullptr || !gameObject->getIsActive()) continue;

		component->fillUserInterfaceRenderQueue(queue);
	}
}

void RenderSystem::setClearColor(const Color& color)
{
	clearColor = color;
}

const std::string RenderSystem::getName() const
{
	return "RenderSystem";
}
const Color& RenderSystem::getClearColor() const
{
	return clearColor;
}
