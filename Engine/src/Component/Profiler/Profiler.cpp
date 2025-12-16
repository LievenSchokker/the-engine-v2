#include "Component/Profiler/Profiler.h"

#include "Core/GameWorld.h"
#include "Rendering/UIRenderCommand.h"
#include "Scene/SceneManager.h"
#include "Rendering/RenderQueue/IUserInterfaceRenderQueueWriter.h"
#include "Rendering/UIRenderCommand.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

Profiler::Profiler(float x, float y, float width, float height)
	: UIElement(x, y, width, height),
	  updateInterval(0.25f),
	  timeSinceLastUpdate(0.0f),
	  currentFPS(0.0f),
	  averageFrameTime(0.0f),
	  minFrameTime(0.0f),
	  maxFrameTime(0.0f),
	  entityCount(0),
	  activeSceneCount(0),
	  showFPS(true),
	  showFrameTime(true),
	  showFrameGraph(true),
	  showEntityCount(true),
	  titleColor(Color::yellow()),
	  labelColor(Color::white()),
	  goodColor(Color::green()),
	  warningColor(Color::yellow()),
	  badColor(Color::red())
{
	panelId = 9999;
}

void Profiler::update(float deltaTime, GameWorld* world)
{
	calculateFPS(deltaTime);

	timeSinceLastUpdate += deltaTime;
	if ( timeSinceLastUpdate >= updateInterval )
	{
		updateStats(world);
		timeSinceLastUpdate = 0.0f;
	}
}

void Profiler::calculateFPS(float deltaTime)
{
	// Real frame time tracking
	frameTimes.push_back(deltaTime);
	if ( frameTimes.size() > kMaxFrameSamples )
	{
		frameTimes.pop_front();
	}

	if ( !frameTimes.empty() )
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
	if ( world != nullptr && world->sceneManager != nullptr )
	{
		entityCount = 0;
		activeSceneCount = 1;
	}
}

void Profiler::fillUserInterfaceRenderQueue(
	IUserInterfaceRenderQueueWriter& queue) const
{
	if ( !visible )
	{
		return;
	}

	queue.push(renderPanel());

	if ( showFPS )
	{
		queue.push(renderFps());
	}

	if ( showFrameTime )
	{
		queue.push(renderFrameTime());
		queue.push(renderMinMaxLabel());
	}

	UIRenderCommand sep1;
	sep1.type = UICommandType::Separator;
	sep1.panelId = panelId;
	queue.push(sep1);

	if ( showEntityCount )
	{
		queue.push(renderEntityLabel());
		queue.push(renderSceneLabel());
	}
}

UIRenderCommand Profiler::renderPanel() const
{
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
	return panel;
}

UIRenderCommand Profiler::renderFps() const
{
	UIRenderCommand fpsLabel;
	fpsLabel.type = UICommandType::Text;
	fpsLabel.panelId = panelId;
	fpsLabel.text = "FPS: " + formatFloat(currentFPS, 1);
	fpsLabel.color = getFPSColor();
	fpsLabel.alignment = Alignment::Left;
	return fpsLabel;
}

UIRenderCommand Profiler::renderFrameTime() const
{
	UIRenderCommand frameTimeLabel;
	frameTimeLabel.type = UICommandType::Text;
	frameTimeLabel.panelId = panelId;
	frameTimeLabel.text =
		"Frame: " + formatFloat(averageFrameTime * 1000.0f, 2) + " ms";
	frameTimeLabel.color = getFrameTimeColor();
	frameTimeLabel.alignment = Alignment::Left;

	return frameTimeLabel;
}

UIRenderCommand Profiler::renderMinMaxLabel() const
{
	UIRenderCommand minMaxLabel;
	minMaxLabel.type = UICommandType::Text;
	minMaxLabel.panelId = panelId;
	minMaxLabel.text = "Min/Max: " + formatFloat(minFrameTime * 1000.0f, 2) +
					   " / " + formatFloat(maxFrameTime * 1000.0f, 2) + " ms";
	minMaxLabel.color = labelColor;
	minMaxLabel.alignment = Alignment::Left;
	return minMaxLabel;
}

UIRenderCommand Profiler::renderEntityLabel() const
{
	UIRenderCommand entityLabel;
	entityLabel.type = UICommandType::Text;
	entityLabel.panelId = panelId;
	entityLabel.text = "Entities: " + std::to_string(entityCount);
	entityLabel.color = labelColor;
	entityLabel.alignment = Alignment::Left;
	return entityLabel;
}

UIRenderCommand Profiler::renderSceneLabel() const
{
	UIRenderCommand sceneLabel;
	sceneLabel.type = UICommandType::Text;
	sceneLabel.panelId = panelId;
	sceneLabel.text = "Active Scenes: " + std::to_string(activeSceneCount);
	sceneLabel.color = labelColor;
	sceneLabel.alignment = Alignment::Left;
	return sceneLabel;
}

void Profiler::toggle()
{
	visible = !visible;
}

void Profiler::setUpdateInterval(float seconds)
{
	updateInterval = seconds;
}

void Profiler::setShowFPS(bool show)
{
	showFPS = show;
}

void Profiler::setShowFrameTime(bool show)
{
	showFrameTime = show;
}

void Profiler::setShowEntityCount(bool show)
{
	showEntityCount = show;
}

Color Profiler::getFPSColor() const
{
	if ( currentFPS >= 60.0f ) return goodColor;
	if ( currentFPS >= 30.0f ) return warningColor;
	return badColor;
}

Color Profiler::getFrameTimeColor() const
{
	if ( averageFrameTime <= 0.016f ) return goodColor;
	if ( averageFrameTime <= 0.033f ) return warningColor;
	return badColor;
}

std::string Profiler::formatFloat(float value, int decimals) const
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(decimals) << value;
	return stream.str();
}