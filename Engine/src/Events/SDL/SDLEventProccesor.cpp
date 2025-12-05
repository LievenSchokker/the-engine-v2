#include "Events/SDL/SDLEventProccesor.h"

#include "Events/ApplicationEvents.h"

namespace Events
{

bool SDLEventProcessor::pollEvents(EventQueue& queue)
{
    return processEvents([&queue](auto event) {
        queue.push(std::move(event));
    });
}

bool SDLEventProcessor::pollEvents(EventDispatcher& dispatcher)
{
    return processEvents([&dispatcher](auto event) {
        dispatcher.dispatch(event);
    });
}

void SDLEventProcessor::setUnhandledEventCallback(
    std::function<void(const SDL_Event&)> callback)
{
    unhandledCallback = std::move(callback);
}

template<typename Handler>
bool SDLEventProcessor::processEvents(Handler handler)
{
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent))
    {
        bool handled = true;

        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                handler(WindowCloseEvent{});
                return false;

            case SDL_WINDOWEVENT:
                switch (sdlEvent.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        handler(WindowResizeEvent{
                            sdlEvent.window.data1,
                            sdlEvent.window.data2
                        });
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        handler(WindowFocusEvent{true});
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        handler(WindowFocusEvent{false});
                        break;

                    case SDL_WINDOWEVENT_CLOSE:
                        handler(WindowCloseEvent{});
                        break;

                    default:
                        handled = false;
                        break;
                }
                break;

            case SDL_KEYDOWN:
                handler(KeyPressedEvent{
                    sdlEvent.key.keysym.sym,
                    sdlEvent.key.keysym.scancode,
                    sdlEvent.key.repeat != 0
                });
                break;

            case SDL_KEYUP:
                handler(KeyReleasedEvent{
                    sdlEvent.key.keysym.sym,
                    sdlEvent.key.keysym.scancode
                });
                break;

            case SDL_TEXTINPUT:
                handler(TextInputEvent{sdlEvent.text.text});
                break;

            case SDL_MOUSEMOTION:
            {
                int deltaX = sdlEvent.motion.xrel;
                int deltaY = sdlEvent.motion.yrel;

                if (!hasLastMousePos)
                {
                    hasLastMousePos = true;
                    deltaX = 0;
                    deltaY = 0;
                }

                lastMouseX = sdlEvent.motion.x;
                lastMouseY = sdlEvent.motion.y;

                handler(MouseMovedEvent{
                    sdlEvent.motion.x,
                    sdlEvent.motion.y,
                    deltaX,
                    deltaY
                });
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
                handler(MouseButtonPressedEvent{
                    sdlEvent.button.button,
                    sdlEvent.button.x,
                    sdlEvent.button.y,
                    sdlEvent.button.clicks
                });
                break;

            case SDL_MOUSEBUTTONUP:
                handler(MouseButtonReleasedEvent{
                    sdlEvent.button.button,
                    sdlEvent.button.x,
                    sdlEvent.button.y
                });
                break;

            case SDL_MOUSEWHEEL:
            {
                float scrollX = static_cast<float>(sdlEvent.wheel.x);
                float scrollY = static_cast<float>(sdlEvent.wheel.y);

                if (sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                {
                    scrollX = -scrollX;
                    scrollY = -scrollY;
                }

                handler(MouseScrollEvent{scrollX, scrollY});
                break;
            }

            default:
                handled = false;
                break;
        }

        if (!handled && unhandledCallback)
        {
            unhandledCallback(sdlEvent);
        }
    }

    return true;
}

}