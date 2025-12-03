#pragma once


#include "External/SdlContext.h"
#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/Color.h"
#include "Scene/Scene.h"

class IRenderer;

class RenderSystem
{
public:
	//TODO create abstract backend
	explicit RenderSystem(SdlContext& context);

	void update(float deltaTime, Scene& scene);
	void setClearColor(const Color& color);

private:
	std::unique_ptr<IRenderer> renderer;
	RenderQueue queue;
	Color clearColor = Color::black();

	void collectCommands(Scene& scene);
};