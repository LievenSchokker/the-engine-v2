#pragma once


#include <cstdint>
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
	 * @return true when accumulator contains at least one full timestep
	 *
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
	 * @brief Gets the fixed timestep duration.
	 * @return Constant delta time for each simulation step
	 *
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
};