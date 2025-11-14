#include <SDL2/SDL.h>

#include <iostream>
#include <memory>

#include "../include/PlaceholderGameObject.h"
#include "../include/SceneManager.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
        std::cout << "SDL can not disable compositor bypass!" << std::endl;
        return 1;
    }
#endif
    SDL_Window* window =
        SDL_CreateWindow("Basic C++ SDL project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SceneManager sceneManager;
    auto prototypeScene = std::make_unique<Scene>("PrototypeScene");
    prototypeScene->addGameObject(std::make_unique<PlaceholderGameObject>("ExampleGameObject"));
    sceneManager.addScene(std::move(prototypeScene));
    sceneManager.setActiveScene("PrototypeScene");

    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = static_cast<float>(currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        // =============== GAME CODE CHECK ===============
        {
            sceneManager.update(deltaTime);
            sceneManager.render();
        }

        SDL_Delay(std::max(0, 16 - static_cast<int>(deltaTime * 1000)));
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
