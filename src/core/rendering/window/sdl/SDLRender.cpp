///
/// Created by Lieven Schokker on 11/11/2025.
///


#include "core/rendering/window/sdl/SDLRender.h"
#include <iostream>
#include <algorithm>
#include <cassert>


SdlWindow::SdlWindow(SdlContext& ctx)
    : windowWidth(0), windowHeight(0) {
    assert(ctx.WasInit(SDL_INIT_VIDEO) && "SDL video subsystem not initialized");
}

SdlWindow::~SdlWindow() {
    close();
}

void SdlWindow::open(const WindowOptions& opts) {
    Uint32 flags = opts.resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_SHOWN;

    window = SDL_CreateWindow(
        opts.title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        opts.width,
        opts.height,
        flags);

    windowWidth = opts.width;
    windowHeight = opts.height;

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        window = nullptr;
        return;
    }
}

bool SdlWindow::isOpen() {
    return window != nullptr;
}

void SdlWindow::close() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
}

void SdlWindow::setTitle(const std::string& title) {
    if (window) SDL_SetWindowTitle(window, title.c_str());
}


void SdlWindow::presentFrame() {
    if (!renderer) return;

    SDL_PumpEvents();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    gridRenderCommands.clear();
    uiRenderCommands.clear();

    SDL_RenderPresent(renderer);
}

void SdlWindow::AddRenderCommand(RenderPackage command) {
    gridRenderCommands.push_back(command);
}

