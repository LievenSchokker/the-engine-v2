#include "Rendering/RenderSystem.h"

#include "../../inc/Rendering/viewport/WorldToCameraViewSpaceAdapter.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Component/UIElement/UIElement.h"
#include "Events/EventImplementations/ApplicationEvents.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"
#include "Rendering/viewport/CameraViewSpaceToScreenSpaceAdapter.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <iostream>

RenderSystem::RenderSystem(std::unique_ptr<IRenderer> renderer)
	: renderer(std::move(renderer)), clearColor(Color::black())
{
}

void RenderSystem::setupEvents(EventDispatcher& dispatcher)
{
	if ( renderer != nullptr )
	{
		renderer->setupEvents(dispatcher);
	}

	subscriptions.push_back(dispatcher.subscribe<WindowResizeEvent>(
		[this](const WindowResizeEvent& event)
		{ this->onWindowResize(event); }));
}

void RenderSystem::onWindowResize(const WindowResizeEvent& event)
{
	windowOptions.height = event.height;
	windowOptions.width  = event.width;

	if (renderer)
	{
		renderer->onResize(event.width, event.height);
	}
}

SystemStatus RenderSystem::start(GameWorld& gameWorld)
{
	windowOptions = gameWorld.specs.renderSettings.windowOptions;
	return SystemStatus::RUNNING;
}

void RenderSystem::update(double deltaTime, const GameWorld& gameWorld)
{
	if ( !renderer || !renderer->isOpen() )
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
	for ( auto& command : queue.world().getCommands() )
	{
		if ( cameras.empty() )
		{
			renderer->execute(command);
		}
		else
		{
			for ( const auto camera : cameras )
			{
				auto transformed =
					WorldToCameraViewSpaceAdapter::Transform(*camera, command);
				renderer->execute(
					CameraViewSpaceToScreenSpaceAdapter::Transform(
						*camera, windowOptions, transformed));
			}
		}
	}
}

void RenderSystem::collectCommands(Scene& scene)
{
	for ( auto* component : scene.getAllComponentsOfType<RenderComponent>() )
	{
		if ( component == nullptr ) continue;

		auto* gameObject = component->getGameObject();
		if ( gameObject == nullptr || !gameObject->getIsActive() ) continue;

		component->fillRenderQueue(queue);
	}

	for ( auto* component :
		  scene.getAllComponentsOfType<UserInterfaceRenderComponent>() )
	{
		if ( component == nullptr ) continue;

		auto* gameObject = component->getGameObject();
		if ( gameObject == nullptr || !gameObject->getIsActive() ) continue;

		component->fillUserInterfaceRenderQueue(queue);
	}
}

void RenderSystem::shutdown(GameWorld& gameWorld)
{
	if ( gameWorld.render == this )
	{
		gameWorld.render = nullptr;
	}

	for ( auto subscription : subscriptions )
	{
		gameWorld.getDispatcher()->unsubscribe(subscription);
	}

	subscriptions.clear();
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

const WindowOptions& RenderSystem::getWindowOptions() const
{
	return windowOptions;
}

void RenderSystem::updateCameras(const Scene& scene)
{
	cameras = scene.getAllComponentsOfType<Camera>();
}
