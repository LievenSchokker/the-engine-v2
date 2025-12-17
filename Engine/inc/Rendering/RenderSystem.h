#pragma once

#include "Component/Camera.h"
#include "Rendering/Color.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Scene/Scene.h"

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
class RenderSystem
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

	/**
	 * @brief Executes a full render frame: collect, sort, draw, present.
	 *
	 * Centralizing the frame lifecycle here guarantees consistent ordering
	 * (e.g., opaque geometry before transparency) and keeps per-frame
	 * boilerplate out of core loop code.
	 *
	 */
	void update(float deltaTime, Scene& scene);
	void processWorldCommands();

	void setClearColor(const Color& color);
	void updateCameras(const Scene& scene);

	const Color& getClearColor() const;

   private:
	std::unique_ptr<IRenderer> renderer;
	std::vector<Camera*> cameras;
	RenderQueue queue;
	Color clearColor = Color::black();
	void collectCommands(Scene& scene);
};