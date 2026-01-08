#pragma once

#include "Core/IEngineSystems.h"
#include "Events/EventDispatcher/EventDispatcher.h"
#include "Input/MouseButton.h"
#include "Events/IEventProccesor.h"

#include <SDL.h>
#include <functional>

/**
 * @brief Processes SDL events and dispatches them as engine events.
 *
 * Polls SDL events during update() and dispatches them through
 * the EventDispatcher. Can also be used standalone via IEventProcessor.
 */
class SDLEventProcessor : public IEngineSystems, public IEventProccesor
{
public:
	SDLEventProcessor() = default;

	SystemStatus start(GameWorld& gameWorld) override;
	void update(double deltaTime, const GameWorld& gameWorld) override;
	void shutdown(GameWorld& gameWorld) override;
	[[nodiscard]] const std::string getName() const override;
	bool pollEvents(EventDispatcher& dispatcher) override;

	/**
	 * @brief Set callback for unhandled SDL events.
	 */
	void setUnhandledEventCallback(
		std::function<void(const SDL_Event&)> callback);

private:
	template <typename Handler>
	bool processEvents(Handler handler);
	MouseButton mouseButtonAdaption(Uint8 button);
	EventDispatcher* internalDispatcher = nullptr;
	std::function<void(const SDL_Event&)> unhandledCallback;

	int lastMouseX = 0;
	int lastMouseY = 0;
	bool hasLastMousePos = false;
};