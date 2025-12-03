#include "External/SDLBackendContext.h"

#include "SDL.h"

#include <stdexcept>

int SDLBackendContext::referenceCount = 0;

SDLBackendContext::SDLBackendContext(Uint32 requestedFlags)
{
	acquire(requestedFlags);
}

SDLBackendContext::~SDLBackendContext()
{
	release();
}

void SDLBackendContext::acquire(Uint32 requestedFlags)
{
	if ( referenceCount == 0 )
	{
		if ( SDL_Init(requestedFlags) != 0 )
		{
			throw std::runtime_error("SDL_Init failed: " +
									 std::string(SDL_GetError()));
		}
	}
	else
	{
		// Initialize any additional subsystems not already initialized
		Uint32 alreadyInit = SDL_WasInit(0);
		Uint32 needInit = requestedFlags & ~alreadyInit;
		if ( needInit != 0 && SDL_InitSubSystem(needInit) != 0 )
		{
			throw std::runtime_error("SDL_InitSubSystem failed: " +
									 std::string(SDL_GetError()));
		}
	}

	++referenceCount;
	flags |= requestedFlags;  // Keep track of requested flags
}

void SDLBackendContext::release()
{
	if ( referenceCount > 0 )
	{
		--referenceCount;
		if ( referenceCount == 0 )
		{
			SDL_Quit();
		}
	}
}

Uint32 SDLBackendContext::initialize()
{
	return SDL_WasInit(0);
}

bool SDLBackendContext::wasInit(Uint32 subsystem) const
{
	return (SDL_WasInit(subsystem) & subsystem) != 0;
}

void SDLBackendContext::shutdown()
{
	// Only actually quit SDL if this is the last reference
	release();
}
