#pragma once

#include "SDL_stdinc.h"

/**
 * @class IBackendContext
 * @brief Abstract interface for backend systems (SDL, GLFW, etc.)
 */
class IBackendContext
{
public:
	virtual ~IBackendContext() = default;

	/// Initialize backend system
	virtual Uint32 initialize() = 0;

	/// Check if a specific subsystem is initialized
	virtual bool wasInit(Uint32 subsystem) const = 0;

	/// Shutdown backend system
	virtual void shutdown() = 0;
};
