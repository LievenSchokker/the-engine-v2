#include "Scene/Scene.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/IRenderer.h"
#include "Rendering/SDL/SDLRenderer.h"

RenderSystem::RenderSystem(SdlContext& context)
	: renderer(std::make_unique<SDLRenderer>(context))
{
}

void RenderSystem::update(float deltaTime, Scene& scene)
{
	if (renderer == nullptr) return;

	renderer->beginFrame(clearColor);

	queue.clear();
	collectCommands(scene);

	// 3. Sort by layer
	queue.sort();

	// 4. Execute each command
	for (const auto& command : queue.getCommands())
	{
		renderer->execute(command);
	}

	// 5. End frame
	renderer->endFrame();
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
}

void RenderSystem::setClearColor(const Color& color)
{
	clearColor = color;
}
