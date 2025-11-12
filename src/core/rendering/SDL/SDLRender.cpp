///
/// Created by Lieven Schokker on 11/11/2025.
///


#include "core/rendering/SDL/SDLRender.h"
#include <iostream>
#include <algorithm>
#include <cassert>

#include "external/SdlContext.h"


SDLRender::SDLRender(SdlContext& ctx)
    : windowWidth(0),
    windowHeight(0)
{
    assert(ctx.wasInit(SDL_INIT_VIDEO) && "SDL video subsystem not initialized");
}

SDLRender::~SDLRender() {
    close();
}

void SDLRender::open(const WindowOptions& opts)
{
    Uint32 flags = SDL_WINDOW_SHOWN;

    window = SDL_CreateWindow(
        opts.title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        opts.width,
        opts.height,
        flags);

    windowWidth = opts.width;
    windowHeight = opts.height;

    if (window == nullptr)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        window = nullptr;
        return;
    }
}

bool SDLRender::isOpen()
{
    return window != nullptr;
}

void SDLRender::close()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void SDLRender::setTitle(const std::string& title)
{
    if (window)
    {
        SDL_SetWindowTitle(window, title.c_str());
    }
}


void SDLRender::presentFrame()
{
    if (!renderer)
    {
        return;
    }

    SDL_PumpEvents();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

