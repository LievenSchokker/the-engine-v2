#pragma once
#ifndef SDL_INPUT_ADAPTER_H
#define SDL_INPUT_ADAPTER_H

#include <SDL2/SDL.h>
#include "KeyCode.h"
#include "MouseButton.h"
#include "IInputAdapter.h"
#include "InputManager.h"

class SDLInputAdapter : public IInputAdapter
{
  public:
    void poll(InputManager& input) override;
};

#endif // SDL_INPUT_ADAPTER_H
