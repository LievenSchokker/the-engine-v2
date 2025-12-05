#pragma once

#include "Behaviour/Behaviour.h"
#include "Component/UIObject/UIObject.h"
#include "Rendering/Color.h"
#include "Rendering/IUIRenderHook.h"

#include <string>
#include <deque>

class GameWorld;

class Profiler : public UIObject, public Behaviour
{
public:
	explicit Profiler(
	   float x = 10.0f,
	   float y = 10.0f,
	   float width = 250.0f,
	   float height = 300.0f
	);

	void fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const override;
	UIRenderCommand renderPanel() const;
	UIRenderCommand renderFps() const;
	UIRenderCommand renderFrameTime() const;
	UIRenderCommand renderMinMaxLabel() const;
	UIRenderCommand renderEntityLabel() const;
	UIRenderCommand renderSceneLabel() const;

	void update(float deltaTime, GameWorld* world) override;

	void toggle();

	void setUpdateInterval(float seconds);
	void setShowFPS(bool show);
	void setShowFrameTime(bool show);
	void setShowEntityCount(bool show);
	void setShowNetworkStats(bool show);
	void setShowPhysicsStats(bool show);

private:
	static constexpr size_t kMaxFrameSamples = 60;

	float updateInterval;
	float timeSinceLastUpdate;

	// FPS calculation
	std::deque<float> frameTimes;
	float currentFPS;
	float averageFrameTime;
	float minFrameTime;
	float maxFrameTime;

	// Cached stats
	int entityCount;
	int activeSceneCount;

	// Display toggles
	bool showFPS;
	bool showFrameTime;
	bool showFrameGraph;
	bool showEntityCount;

	// Colors
	Color titleColor;
	Color labelColor;
	Color goodColor;
	Color warningColor;
	Color badColor;

	void updateStats(GameWorld* world);
	void calculateFPS(float deltaTime);
	Color getFPSColor() const;
	Color getFrameTimeColor() const;
	std::string formatFloat(float value, int decimals) const;
};