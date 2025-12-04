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

	void update(float deltaTime, GameWorld* world) override;

	void toggle();

	void setUpdateInterval(float seconds);
	void setShowFPS(bool show);
	void setShowFrameTime(bool show);
	void setShowEntityCount(bool show);
	void setShowNetworkStats(bool show);
	void setShowPhysicsStats(bool show);

private:
	float updateInterval = 0.25f;
	float timeSinceLastUpdate = 0.0f;

	// FPS calculation
	std::deque<float> frameTimes;
	static constexpr size_t kMaxFrameSamples = 60;
	float currentFPS = 0.0f;
	float averageFrameTime = 0.0f;
	float minFrameTime = 0.0f;
	float maxFrameTime = 0.0f;

	// Cached stats
	int entityCount = 0;
	int activeSceneCount = 0;
	std::string networkStatus = "Offline";
	int networkPing = 0;
	int connectedClients = 0;
	int localClientId = -1;
	bool isServer = false;
	bool isClient = false;

	// Display toggles
	bool showFPS = true;
	bool showFrameTime = true;
	bool showEntityCount = true;
	bool showNetworkStats = true;
	bool showPhysicsStats = true;

	// Colors
	Color titleColor = Color::yellow();
	Color labelColor = Color::white();
	Color goodColor = Color::green();
	Color warningColor = Color::yellow();
	Color badColor = Color::red();

	void updateStats(GameWorld* world);
	void calculateFPS(float deltaTime);
	Color getFPSColor() const;
	Color getFrameTimeColor() const;
	std::string formatFloat(float value, int decimals) const;
};