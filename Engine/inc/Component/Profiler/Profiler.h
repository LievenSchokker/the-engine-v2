#pragma once


#include "Behaviour/Behaviour.h"
#include "Component/UIElement/UIElement.h"
#include "Rendering/Color.h"
#include "Rendering/IUIRenderHook.h"

#include <string>
#include <deque>

class GameWorld;

/**
 * @brief Debug overlay displaying real-time performance metrics.
 *
 * Inherits from both UIObject and Behaviour: UIObject provides the rendering
 * interface while Behaviour hooks into the update loop for sampling. This
 * dual inheritance keeps the profiler self-contained—no external system
 * needs to know it exists beyond adding it to the scene.
 *
 * Stats are cached and refreshed at a configurable interval rather than
 * every frame to avoid the profiler itself skewing measurements and to
 * keep the displayed values readable (rapidly changing numbers are hard
 * to follow).
 */
class Profiler  final : public UIElement, public Behaviour
{
public:
	explicit Profiler(
		float x = 10.0f,
		float y = 10.0f,
		float width = 250.0f,
		float height = 300.0f
		);

	void fillUserInterfaceRenderQueue(
		IUserInterfaceRenderQueueWriter& queue) const override;

	/// @name Individual render commands
	/// @brief Split into separate methods so subclasses or tests can
	///        override or verify specific panel sections independently.
	/// @{
	UIRenderCommand renderPanel() const;
	UIRenderCommand renderFps() const;
	UIRenderCommand renderFrameTime() const;
	UIRenderCommand renderMinMaxLabel() const;
	/// @}


	void update(double deltaTime, const GameWorld& world) override;
	void toggle();

	void setUpdateInterval(float seconds);
	void setShowFPS(bool show);
private:
	/// Rolling window size for frame time averaging. 60 samples smooths
	/// out single-frame spikes while still responding to sustained changes.
	static constexpr size_t kMaxFrameSamples = 60;

	float updateInterval;
	float timeSinceLastUpdate;

	/// @name FPS calculation
	/// @brief Uses a deque as a fixed-size sliding window—cheap to push/pop
	///        at both ends without reallocating.
	/// @{
	std::deque<float> frameTimes;
	float currentFPS;
	float averageFrameTime;
	float minFrameTime;
	float maxFrameTime;
	/// @}

	bool showFPS;
	bool showFrameTime;
	bool showFrameGraph;
	bool showEntityCount;

	/// @name Status colors
	/// @brief Provide immediate visual feedback: green/yellow/red thresholds
	///        let developers spot performance problems at a glance without
	///        reading exact numbers.
	/// @{
	Color titleColor;
	Color labelColor;
	Color goodColor;
	Color warningColor;
	Color badColor;
	/// @}

	void calculateFPS(float deltaTime);
	Color getFPSColor() const;
	Color getFrameTimeColor() const;
	std::string formatFloat(float value, int decimals) const;
};