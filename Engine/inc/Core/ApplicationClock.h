#pragma once


#include <functional>

/**
 * @class ApplicationClock
 * @brief frame timing with fixed timestep accumulation for deterministic simulation.
 *
 * Implements the "Fix Your Timestep" from: https://gafferongames.com/post/fix_your_timestep/
 * This ensures physics and game logic run at a tick rate
 * making deterministic networking via lockstep synchronization possible.
 */
class ApplicationClock
{
public:
	using ClockFunction = std::function<double()>;

	/**
	 * @brief Constructs a timer with fixed simulation timestep.
	 * @param fixedDeltaTime Time step for simulation updates
	 * @param clockFunc Platform-specific clock function providing monotonic time
	 *
	 * @note Fixed timestep ensures reproducible simulation behavior across different
	 *       hardware and prevents the "spiral of death" where slow frames cause more
	 *       updates which cause even slower frames.
	 */
	ApplicationClock(const ClockFunction& clockFunc,
	                 int tickrate, double maxAccumulatedTime);

	/**
	 * @brief Initializes timing state at application start.
	 *
	 * @note Must be called before tick() to establish time baseline and prevent
	 *       massive initial frameTime from causing spiral of death.
	 */
	void start();

	/**
	 * @brief Accumulates frame time for consumption by fixed updates.
	 *
	 * @note Called once per render frame. Clamps extreme frame times to prevent
	 *       spiral of death when simulation can't keep up with real time.
	 *       The accumulator carries leftover time between frames to get
	 *       accurate simulation speed even with high framerate.
	 */
	void tick();

	/**
	 * @brief Checks if enough time has accumulated for another fixed update.
	 * @return true when accumulator contains at least one full timestep and not paused
	 *
	 * @note Returns false when paused, preventing simulation updates.
	 */
	bool shouldFixedUpdate() const;

	/**
	 * @brief Consumes one fixed timestep from the accumulator.
	 *
	 * @note Must be called after each fixed update to prevent infinite loops.
	 *       Advances simulation time and tick counter, tracking how many
	 *       simulation steps have occurred independently of rendering.
	 */
	void consumeFixedUpdate();

	/**
	 * @brief Calculates interpolation factor between physics states.
	 * @return Alpha value [0,1] representing progress toward next physics step
	 *
	 */
	double getAlpha() const;

	/**
	 * @brief Gets total elapsed simulation time.
	 * @return Accumulated time in fixed timestep increments
	 *
	 */
	double getTime() const;

	/**
	 * @brief Gets the fixed timestep duration, scaled by time scale.
	 * @return Scaled delta time for each simulation step (fixedDeltaTime * timeScale)
	 *
	 * @note This returns the scaled delta time, which affects simulation speed.
	 *       Use this for all game logic and physics updates to respect debug time controls.
	 */
	double getDeltaTime() const;

	/**
	 * @brief Gets remaining time in accumulator after fixed updates.
	 * @return Leftover frame time not yet simulated (always < fixedDeltaTime)
	 *
	 */
	double getAccumulatedTime() const;

	/**
	 * @brief Gets total number of fixed updates executed.
	 * @return Monotonically increasing tick counter
	 *
	 */
	int getTotalTicks() const;

	/**
	 * @brief Sets the time scale multiplier for simulation speed.
	 * @param scale Time scale multiplier (1.0 = normal, 0.5 = slow, 2.0 = fast)
	 *
	 * @note Time scale affects getDeltaTime(), effectively speeding up or slowing down
	 *       the entire simulation. Rendering continues at normal speed.
	 */
	void setTimeScale(double scale);

	/**
	 * @brief Gets the current time scale multiplier.
	 * @return Current time scale (default: 1.0)
	 */
	double getTimeScale() const;

	/**
	 * @brief Pauses the simulation (fixed updates).
	 *
	 * @note When paused, shouldFixedUpdate() returns false, preventing simulation
	 *       updates. Rendering and input continue to function normally.
	 */
	void pause();

	/**
	 * @brief Resumes a paused simulation.
	 */
	void resume();

	/**
	 * @brief Toggles pause state.
	 */
	void togglePause();

	/**
	 * @brief Checks if the simulation is currently paused.
	 * @return true when paused, false otherwise
	 */
	bool isPaused() const;

private:
	/** @brief A functions that retusn, the time the applicationhas beenrunning in second */
	ClockFunction getClock;
	double fixedDeltaTime;
	double currentTime;
	double accumulatedTime;
	double simulationTime;
	int totalTicks;

	int tickRate;
	double maxAccumulatedTime;

	// Debug controls
	double timeScale;  // Multiplier for simulation speed (1.0 = normal)
	bool paused;       // Whether simulation is paused
};