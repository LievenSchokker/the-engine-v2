/**
* @file SdlContext.h
 * @brief RAII wrapper for SDL initialization and cleanup
 * @author Lieven Schokker
 * @date 11/11/2025
 */

/**
 * @class SdlContext
 * @brief Resource Acquisition Is Initialization (RAII) wrapper for SDL subsystems
 *
 * This class manages SDL initialization and cleanup using reference counting,
 * ensuring SDL is properly initialized when the first instance is created and
 * cleaned up when the last instance is destroyed. Multiple instances can safely
 * coexist, and each can request different subsystems.
 *
 * @note This class uses reference counting to manage the lifetime of SDL initialization
 * @warning Not thread-safe. External synchronization required for concurrent use.
 *
 */


#pragma once
#include <SDL.h>


class SdlContext {
public:
    /**
     * @brief Constructs an SdlContext and initializes requested SDL subsystems
     *
     * If this is the first instance, initializes SDL with the requested flags.
     * If SDL is already initialized, only initializes the additional subsystems
     * that weren't already initialized.
     *
     * @param flags SDL subsystem flags to initialize (default: SDL_INIT_EVERYTHING)
     * @throws std::runtime_error if SDL initialization fails
     *
     * @see SDL_Init, SDL_InitSubSystem
     */
    explicit SdlContext(Uint32 flags = SDL_INIT_EVERYTHING);

    /**
     * @brief Destroys the SdlContext
     *
     * calls SDL_Quit() to clean up all SDL subsystems.
     *
     * @see SDL_Quit
     */
    ~SdlContext();

    /**
     * @brief Gets the currently initialized SDL subsystem flags
     *
     * @return Bitmask of all currently initialized SDL subsystems
     * @see SDL_WasInit
     */
    static Uint32 initFlags();

    /**
     * @brief Checks if a specific SDL subsystem is initialized
     *
     * @param subsystem The SDL subsystem flag to check (e.g., SDL_INIT_VIDEO)
     * @return true if the subsystem is initialized, false otherwise
     * @see SDL_WasInit
     */
    static bool wasInit(Uint32 subsystem);

private:
    /**
     * @brief Initializes SDL subsystems and increments reference count
     *
     * Initializes the requested SDL subsystems. If SDL is not yet initialized,
     * calls SDL_Init(). Otherwise, calls SDL_InitSubSystem() for any subsystems
     * not already initialized.
     *
     * @param flags SDL subsystem flags to initialize
     * @throws std::runtime_error if SDL initialization fails
     */
    void acquire(Uint32 flags);

    /**
     * @brief Decrements reference count and cleans up if last instance
     *
     * Decrements the reference count. If this was the last instance,
     * calls SDL_Quit() to clean up all SDL subsystems.
     */
    void release();

    Uint32 flags = 0;           ///< SDL subsystem flags requested by this instance

    static int referenceCount;  ///< Number of active SdlContext instances
    static Uint32 globalFlags;  ///< Currently initialized SDL subsystem flags (global state)
};