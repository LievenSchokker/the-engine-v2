#pragma once

#include "Events/EventQueue.h"
#include "Events/Event.h"
#include "Events/IEventProccesor.h"
#include "Input/MouseButton.h"

#include <SDL.h>
#include <functional>


/**
 * @brief Processes SDL events and converts them to engine events
 *
 * Can either dispatch events immediately or queue them for
 * deferred processing.
 */
class SDLEventProcessor: IEventProccesor
{
public:
	SDLEventProcessor() = default;
	/**
	 * @brief Poll and process all SDL events, queuing them
	 * @param queue Event queue to push events into
	 * @return false if application should quit
	 */
	bool pollEvents(EventQueue& queue) override;

	/**
	 * @brief Poll and process all SDL events, dispatching immediately
	 * @param dispatcher Event dispatcher
	 * @return false if application should quit
	 */
	bool pollEvents(EventDispatcher& dispatcher) override;

	/**
	 * @brief Set callback for unhandled SDL events
	 *
	 * Use this to handle SDL events that aren't converted to
	 * engine events (e.g., controller events, drop events).
	 */
	void setUnhandledEventCallback(
		std::function<void(const SDL_Event&)> callback);

private:
	template <typename Handler>
	bool processEvents(Handler handler);
	MouseButton MouseButtonAdaption(Uint8 button);

	std::function<void(const SDL_Event&)> unhandledCallback;

	// Track previous mouse position for delta calculation
	int lastMouseX = 0;
	int lastMouseY = 0;
	bool hasLastMousePos = false;
};