#include <SDL2/SDL.h>
#include <iostream>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

#include "Input/InputManager.h"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        std::cout << "SDL can not disable compositor bypass!" << std::endl;
        return 0;
    }
#endif
    SDL_Window *window =
        SDL_CreateWindow("Basic C++ SDL project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cout << "Window could not be created!" << std::endl << "SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    bool running = true;

    InputManager *input = InputManager::getInstance();
    std::cout << "\nTry the following:\n" << std::endl;
    std::cout << "- Press W and A to see if they are currently held" << std::endl;
    std::cout << "- Press the left mouse button and the right mouse button to see if "
                 "they are currently held and if the right mouse button was "
                 "pressed"
              << std::endl;
    std::cout << "- Move the mouse to see if it moved this frame and is close "
                 "to the top left corner (x < 20 && y < 20)"
              << std::endl;
    std::cout << "- Scroll the mouse wheel to see if the wheel delta is not 0" << std::endl;
    std::cout << "- Press the escape key to quit" << std::endl;

    while (running)
    {
        input->update();

        // ================================ GAME CODE CHECK
        // ================================
        {

            if (input->isKeyDown(KeyCode::W) && input->isKeyDown(KeyCode::A))
            {
                std::cout << "W and A are currently held (game code check)" << std::endl;
            }
            // =================================================================================
            if (input->isMouseDown(MouseButton::LEFT) && input->wasMousePressed(MouseButton::RIGHT))
            {
                std::cout << "Left mouse button is currently held and right "
                             "mouse button was pressed (game code check)"
                          << std::endl;
            }
            // =================================================================================
            if (input->mouseMovedThisFrame() && input->mouseX() < 20 && input->mouseY() < 20)
            {
                std::cout << "Mouse moved this frame and is close to the top "
                             "left corner (game code check)"
                          << std::endl;
            }
            // =================================================================================
            if (input->wheelDeltaX() != 0 || input->wheelDeltaY() != 0)
            {
                std::cout << "Mouse wheel delta: (" << input->wheelDeltaX() << ", " << input->wheelDeltaY() << ")" << std::endl;
            }
            // =================================================================================
            if (input->wasKeyPressed(KeyCode::ESCAPE))
            {
                std::cout << "Escape key was pressed (game code check)" << std::endl;
                running = false;
            }
        }

        // OS quit button was pressed
        if (input->quitRequested())
        {
            running = false;
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
