//
// Created by Lieven Schokker on 11/11/2025.
//


#include "../../include/external/SdlContext.h"
#include <string>


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
	this->flags = requestedFlags;
}

void SdlContext::release()
{
	--referenceCount;

	if (referenceCount == 0)
	{
		SDL_Quit();
		globalFlags = 0;
	}

	isOwner = false;
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
