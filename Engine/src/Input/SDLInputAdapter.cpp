#include "Input/SDLInputAdapter.h"

void SDLInputAdapter::poll(InputManager& input)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            input.signalQuit();
            break;
        case SDL_KEYDOWN:
            if (event.key.repeat == 0)
            {
                input.setKeyDown(
                    static_cast<KeyCode>(event.key.keysym.scancode), true);
            }
            break;
        case SDL_KEYUP:
            input.setKeyDown(static_cast<KeyCode>(event.key.keysym.scancode),
                             false);
            break;
        case SDL_MOUSEMOTION:
            input.setMousePosition(event.motion.x, event.motion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            input.setMouseDown(static_cast<MouseButton>(event.button.button),
                               true);
            break;
        case SDL_MOUSEBUTTONUP:
            input.setMouseDown(static_cast<MouseButton>(event.button.button),
                               false);
            break;
        case SDL_MOUSEWHEEL:
            input.addMouseWheelDelta(event.wheel.x, event.wheel.y);
            break;
        default:
            break;
        }
    }
}
