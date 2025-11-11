/**
 * @brief Acquires and initializes SDL subsystems with reference counting
 *
 * This function initializes the requested SDL subsystems
 * On the first call (when referenceCount is 0), it performs a full SDL_Init.
 * On subsequent calls, it only initializes subsystems that weren't previously
 * initialized using SDL_InitSubSystem.
 *
 * @param requestedFlags SDL subsystem flags to initialize
 *
 * @throws std::runtime_error If SDL_Init or SDL_InitSubSystem fails returns SDL_GetError().
 *
 *
 */


#pragma once
#include <SDL.h>


class SdlContext {

public:
	explicit SdlContext(Uint32 flags = SDL_INIT_EVERYTHING);
	~SdlContext();

	static Uint32 initFlags();
	static bool wasInit(Uint32 subsystem);

private:
	void acquire(Uint32 flags);
	void release();

	Uint32 flags = 0;
	bool isOwner = false;

	static int referenceCount;
	static Uint32 globalFlags;
};

