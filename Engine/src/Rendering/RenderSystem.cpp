#include "Rendering/RenderSystem.h"

#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/UIElement/UIElement.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/ViewAdapters/WorldToCameraSpaceAdapter.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>

RenderSystem::RenderSystem(std::unique_ptr<IRenderer> renderer)
	: renderer(std::move(renderer))
{
}



void RenderSystem::setupEvents(EventDispatcher& dispatcher) const
{
	if (renderer != nullptr)
	{
		renderer->setupEvents(dispatcher);
	}
}

void RenderSystem::update(double deltaTime, const GameWorld& gameWorld)
{
	if (!renderer || !renderer->isOpen())
	{
		return;
	}

	updateCameras(*gameWorld.sceneManager->getActiveScene());
	queue.clearAll();
	collectCommands(*gameWorld.sceneManager->getActiveScene());
	queue.sortAll();
	renderer->beginFrame(clearColor);

	processWorldCommands();

	renderer->submitUI(queue.ui().getCommands(), *gameWorld.input);
	renderer->endFrame();
}

void RenderSystem::processWorldCommands()
{
	for (auto& command : queue.world().getCommands())
	{
		if (cameras.empty())
		{
			renderer->execute(command);
		}
		else
		{
			for (const auto camera : cameras)
			{
				//Transform returns optional because camera culling happens here
				if (auto transformed = WorldToCameraSpaceAdapter::Transform(
					*camera, command); transformed.has_value())
				{
					renderer->execute(transformed.value());
				}
			}
		}
	}
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

void RenderSystem::updateCameras(const Scene& scene)
{
	cameras = scene.getAllComponentsOfType<Camera>();
}