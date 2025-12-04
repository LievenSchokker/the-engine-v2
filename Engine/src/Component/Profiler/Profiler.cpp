#include "Component/Profiler/Profiler.h"

#include "Rendering/UIRenderCommand.h"
#include "Core/GameWorld.h"
#include "Scene/SceneManager.h"

#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <iostream>

Profiler::Profiler(float x, float y, float width, float height)
    : UIObject(x, y, width, height)
{
    panelId = 9999;
}

void Profiler::update(float deltaTime, GameWorld* world)
{
	calculateFPS(deltaTime);

	timeSinceLastUpdate += deltaTime;
	if (timeSinceLastUpdate >= updateInterval)
	{
		updateStats(world);
		timeSinceLastUpdate = 0.0f;
	}
}

void Profiler::calculateFPS(float deltaTime)
{
    frameTimes.push_back(deltaTime);

    if (frameTimes.size() > kMaxFrameSamples)
    {
        frameTimes.pop_front();
    }

    if (!frameTimes.empty())
    {
        float sum = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0f);
        averageFrameTime = sum / static_cast<float>(frameTimes.size());
        currentFPS = averageFrameTime > 0.0f ? 1.0f / averageFrameTime : 0.0f;

        minFrameTime = *std::min_element(frameTimes.begin(), frameTimes.end());
        maxFrameTime = *std::max_element(frameTimes.begin(), frameTimes.end());
    }
}

void Profiler::updateStats(GameWorld* world)
{
    if (world == nullptr)
    {
        networkStatus = "No World";
        isServer = false;
        isClient = false;
        return;
    }

    // Cache values for rendering
    isServer = world->isServer();
    isClient = world->isClient();
    localClientId = world->localClientId;

    // Entity count from SceneManager
    if (world->sceneManager != nullptr)
    {
        // Adjust based on your SceneManager API
        entityCount = 0;
        activeSceneCount = 1;
    }

    // Network stats
    if (isServer && world->server != nullptr)
    {
        networkStatus = "Server";
        connectedClients = 0;
    }
    else if (isClient && world->client != nullptr)
    {
        networkStatus = "Client";
        networkPing = 0;
    }
    else
    {
        networkStatus = "Offline";
    }
}

void Profiler::fillUserInterfaceRenderQueue(IUserInterfaceRenderQueueWriter& queue) const
{
    if (!visible)
    {
        return;
    }

    // Panel
    UIRenderCommand panel;
    panel.type = UICommandType::Panel;
    panel.panelId = panelId;
    panel.parentId = NO_PARENT;
    panel.x = x;
    panel.y = y;
    panel.width = width;
    panel.height = height;
    panel.title = "Debug Profiler";
    panel.hasTitle = true;
    panel.hasBorder = true;
    panel.rowHeight = 20.0f;
    panel.columns = 1;
    queue.push(panel);

    // FPS
    if (showFPS)
    {
        UIRenderCommand fpsLabel;
        fpsLabel.type = UICommandType::Text;
        fpsLabel.panelId = panelId;
        fpsLabel.text = "FPS: " + formatFloat(currentFPS, 1);
        fpsLabel.color = getFPSColor();
        fpsLabel.alignment = Alignment::Left;
        queue.push(fpsLabel);
    }

    // Frame Time
    if (showFrameTime)
    {
        UIRenderCommand frameTimeLabel;
        frameTimeLabel.type = UICommandType::Text;
        frameTimeLabel.panelId = panelId;
        frameTimeLabel.text = "Frame: " + formatFloat(averageFrameTime * 1000.0f, 2) + " ms";
        frameTimeLabel.color = getFrameTimeColor();
        frameTimeLabel.alignment = Alignment::Left;
        queue.push(frameTimeLabel);

        UIRenderCommand minMaxLabel;
        minMaxLabel.type = UICommandType::Text;
        minMaxLabel.panelId = panelId;
        minMaxLabel.text = "Min/Max: " + formatFloat(minFrameTime * 1000.0f, 2) +
                          " / " + formatFloat(maxFrameTime * 1000.0f, 2) + " ms";
        minMaxLabel.color = labelColor;
        minMaxLabel.alignment = Alignment::Left;
        queue.push(minMaxLabel);
    }

    // Separator
    UIRenderCommand sep1;
    sep1.type = UICommandType::Separator;
    sep1.panelId = panelId;
    queue.push(sep1);

    // Entity Count
    if (showEntityCount)
    {
        UIRenderCommand entityLabel;
        entityLabel.type = UICommandType::Text;
        entityLabel.panelId = panelId;
        entityLabel.text = "Entities: " + std::to_string(entityCount);
        entityLabel.color = labelColor;
        entityLabel.alignment = Alignment::Left;
        queue.push(entityLabel);

        UIRenderCommand sceneLabel;
        sceneLabel.type = UICommandType::Text;
        sceneLabel.panelId = panelId;
        sceneLabel.text = "Active Scenes: " + std::to_string(activeSceneCount);
        sceneLabel.color = labelColor;
        sceneLabel.alignment = Alignment::Left;
        queue.push(sceneLabel);
    }

    // Separator
    UIRenderCommand sep2;
    sep2.type = UICommandType::Separator;
    sep2.panelId = panelId;
    queue.push(sep2);

    // Network
    if (showNetworkStats)
    {
        UIRenderCommand netStatusLabel;
        netStatusLabel.type = UICommandType::Text;
        netStatusLabel.panelId = panelId;
        netStatusLabel.text = "Network: " + networkStatus;

        if (networkStatus == "Server")
        {
            netStatusLabel.color = Color(0, 255, 255, 255);  // Cyan
        }
        else if (networkStatus == "Client")
        {
            netStatusLabel.color = goodColor;
        }
        else
        {
            netStatusLabel.color = Color(150, 150, 150, 255);
        }

        netStatusLabel.alignment = Alignment::Left;
        queue.push(netStatusLabel);

        if (isServer)
        {
            UIRenderCommand clientsLabel;
            clientsLabel.type = UICommandType::Text;
            clientsLabel.panelId = panelId;
            clientsLabel.text = "Clients: " + std::to_string(connectedClients);
            clientsLabel.color = labelColor;
            clientsLabel.alignment = Alignment::Left;
            queue.push(clientsLabel);
        }
        else if (isClient)
        {
            UIRenderCommand pingLabel;
            pingLabel.type = UICommandType::Text;
            pingLabel.panelId = panelId;
            pingLabel.text = "Ping: " + std::to_string(networkPing) + " ms";

            if (networkPing < 50)
            {
                pingLabel.color = goodColor;
            }
            else if (networkPing < 100)
            {
                pingLabel.color = warningColor;
            }
            else
            {
                pingLabel.color = badColor;
            }

            pingLabel.alignment = Alignment::Left;
            queue.push(pingLabel);
        }

        UIRenderCommand clientIdLabel;
        clientIdLabel.type = UICommandType::Text;
        clientIdLabel.panelId = panelId;
        clientIdLabel.text = "Local ID: " + std::to_string(localClientId);
        clientIdLabel.color = labelColor;
        clientIdLabel.alignment = Alignment::Left;
        queue.push(clientIdLabel);
    }
}

void Profiler::toggle()
{
    visible = !visible;
}

void Profiler::setUpdateInterval(float seconds) { updateInterval = seconds; }
void Profiler::setShowFPS(bool show) { showFPS = show; }
void Profiler::setShowFrameTime(bool show) { showFrameTime = show; }
void Profiler::setShowEntityCount(bool show) { showEntityCount = show; }
void Profiler::setShowNetworkStats(bool show) { showNetworkStats = show; }
void Profiler::setShowPhysicsStats(bool show) { showPhysicsStats = show; }

Color Profiler::getFPSColor() const
{
    if (currentFPS >= 60.0f) return goodColor;
    if (currentFPS >= 30.0f) return warningColor;
    return badColor;
}

Color Profiler::getFrameTimeColor() const
{
    if (averageFrameTime <= 0.016f) return goodColor;
    if (averageFrameTime <= 0.033f) return warningColor;
    return badColor;
}

std::string Profiler::formatFloat(float value, int decimals) const
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    return stream.str();
}