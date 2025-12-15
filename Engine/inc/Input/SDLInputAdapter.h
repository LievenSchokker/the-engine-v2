#pragma once


#ifndef SDL_INPUT_ADAPTER_H
#define SDL_INPUT_ADAPTER_H

#include "IInputAdapter.h"
#include "InputManager.h"

class SDLInputAdapter: public IInputAdapter
{
public:
	void poll(InputManager& input) override;
};

#endif // SDL_INPUT_ADAPTER_H