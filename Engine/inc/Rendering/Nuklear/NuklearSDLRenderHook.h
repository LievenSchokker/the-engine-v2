#pragma once


#include <SDL_video.h>

#include "Input/InputManager.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/SDL/SDLRenderer.h"

class NuklearSDLRenderHook : public IUIRenderHook
{
    public:
    NuklearSDLRenderHook(SDL_Window* window, SDL_Renderer* renderer);
    void updateInput() override;

    void initialize() override;
    void beginFrame() override;
    void render() const;
    void presentFrame() override;
    void close() override;

private:
    InputManager* inputManager;
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    struct nk_context* nkCtx;
};
