//
// Created by Lieven Schokker on 11/11/2025.
//




#include <string>
#include <stdexcept>
#include <iostream>

#include "external/SdlContext.h"


int SdlContext::referenceCount = 0;
Uint32 SdlContext::globalFlags = 0;

SdlContext::SdlContext(Uint32 flags)
{
	acquire(flags);
}

SdlContext::~SdlContext()
{
	release();
}

void SdlContext::acquire(Uint32 requestedFlags)
{

	#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
	// Disable compositor bypass
	if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
	{
		std::cout << "SDL can not disable compositor bypass!" << std::endl;
		return;
	}
	#endif

	if (referenceCount == 0)
	{
		if (SDL_Init(requestedFlags) != 0)
		{
			throw std::runtime_error(
				std::string("SDL_Init failed: ") + SDL_GetError()
			);
		}

		globalFlags = SDL_WasInit(0);
	} else
	{
		Uint32 alreadyInitialized = SDL_WasInit(0);
		Uint32 needToInit = requestedFlags & ~alreadyInitialized;

		if (needToInit != 0)
		{
			if (SDL_InitSubSystem(needToInit) != 0)
			{
				throw std::runtime_error(
					std::string("SDL_InitSubSystem failed: ") + SDL_GetError()
				);
			}
			globalFlags = SDL_WasInit(0);
		}
	}

	++referenceCount;
	flags = requestedFlags;
}

void SdlContext::release()
{
	--referenceCount;

	if (referenceCount == 0)
	{
		SDL_Quit();
		globalFlags = 0;
	}

	flags = 0;
}

Uint32 SdlContext::initFlags()
{
	return SDL_WasInit(0);
}

bool SdlContext::wasInit(Uint32 subsystem)
{
	return (SDL_WasInit(subsystem) & subsystem) != 0;
}
