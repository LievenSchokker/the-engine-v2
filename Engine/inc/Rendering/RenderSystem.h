#pragma once

#include "Component/Camera.h"
#include "Core/IEngineSystems.h"
#include "Events/EventDispatcher/EventDispatcher.h"
#include "Events/EventImplementations/ApplicationEvents.h"
#include "External/SDLBackendContext.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Scene/Scene.h"
#include "viewport/viewportConfig.h"

class IRenderer;

/**
 * @brief Orchestrates the rendering pipeline while abstracting backend details.
 *
 * RenderSystem acts as a boundary between game logic and graphics API
 * specifics. By collecting all render commands into a queue before submission,
 * it ensures draw calls happen in a predictable order regardless of how
 * entities are organized in the scene.
 *
 */
class RenderSystem: public IEngineSystems
{
   public:
	/**
	 * @brief Takes ownership of a renderer implementation.
	 * @param renderer The backend-specific renderer to delegate drawing to.
	 *
	 * Dependency injection allows tests to substitute a mock renderer and
	 * enables runtime selection of graphics backends.
	 *
	 */
	explicit RenderSystem(std::unique_ptr<IRenderer> renderer);
	void setupEvents(EventDispatcher& dispatcher);

	// #TODO Mag weg?
	void setupEvents(EventDispatcher& dispatcher) const;
	void onWindowResize(const WindowResizeEvent& event);

	SystemStatus start(GameWorld& gameWorld) override;

	/**
	 * @brief Executes a full render frame: collect, sort, draw, present.
	 *
	 * Centralizing the frame lifecycle here guarantees consistent ordering
	 * (e.g., opaque geometry before transparency) and keeps per-frame
	 * boilerplate out of core loop code.
	 *
	 */
	void update(double deltaTime, const GameWorld& gameWorld) override;
	void shutdown(GameWorld& gameWorld) override;
	void processWorldCommands();
	void setClearColor(const Color& color);
	void updateCameras(const Scene& scene);

	const Color& getClearColor() const;

   private:
	const std::string getName() const override;
	WindowOptions windowOptions;
	std::unique_ptr<IRenderer> renderer;
	std::vector<Camera*> cameras;
	RenderQueue queue;
	Color clearColor = Color::white();
	void collectCommands(Scene& scene);
	std::vector<SubscriptionHandle> subscriptions;
};