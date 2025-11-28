#include "Component/ShapeRenderer.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Rendering/Color.h"
#include "Rendering/IRenderer.h"
#include "Rendering/RenderQueue.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include <gtest/gtest.h>

namespace
{
struct FakeRenderer: public IRenderer {
	void open(const WindowOptions&) override
	{
	}

	void beginFrame(const Color& color) override
	{
		lastClearColor = color;
		++beginCalls;
	}

	void presentFrame() override
	{
		++presentCalls;
	}

	void close() override
	{
		isWindowOpen = false;
	}

	bool isOpen() override
	{
		return isWindowOpen;
	}

	void setTitle(const std::string&) override
	{
	}

	void drawCircle(const Vector2& center, double radius, const Color& color,
					const Vector2& scale) override
	{
		++circleCalls;
		lastCircleCenter = center;
		lastCircleRadius = radius;
		lastCircleColor = color;
		lastCircleScale = scale;
	}

	void drawRectangle(const Vector2& center, const Vector2& size,
					   double rotationDegrees, const Color& color,
					   const Vector2& scale) override
	{
		++rectangleCalls;
		lastRectCenter = center;
		lastRectSize = size;
		lastRectRotation = rotationDegrees;
		lastRectColor = color;
		lastRectScale = scale;
	}

	bool isWindowOpen = true;
	int beginCalls = 0;
	int presentCalls = 0;
	int circleCalls = 0;
	int rectangleCalls = 0;
	Color lastClearColor = Color::black();
	Vector2 lastCircleCenter{};
	double lastCircleRadius = 0.0;
	Color lastCircleColor = Color::black();
	Vector2 lastCircleScale{};
	Vector2 lastRectCenter{};
	Vector2 lastRectSize{};
	double lastRectRotation = 0.0;
	Color lastRectColor = Color::black();
	Vector2 lastRectScale{};
};
}  // namespace

// Check that SceneManager correctly renders a GameObject with a circle
// ShapeRenderer
TEST(ShapeRendererTest, RendersCircleShapeThroughSceneManager)
{
	// Arrange
	FakeRenderer renderer;
	SceneManager manager;
	manager.setClearColor(Color::black());

	auto scene = std::make_unique<Scene>("ShapeScene");
	auto circle = std::make_unique<GameObject>();
	circle->getTransform()->setPosition({42.0, 24.0});
	circle->getTransform()->setScale({1.0, 1.0});
	circle->addComponent<ShapeRenderer>()->setCircle(25.0).setColor(
		Color::blue());
	scene->addGameObject(std::move(circle));

	manager.addScene(std::move(scene));
	manager.setActiveScene("ShapeScene");

	// Act
	RenderQueue queue;
	manager.buildRenderQueue(queue);
	executeRenderQueue(renderer, queue);

	// Assert - Verify frame lifecycle was called
	EXPECT_EQ(renderer.beginCalls, 1);
	EXPECT_EQ(renderer.presentCalls, 1);

	// Assert - Verify circle was drawn, not rectangle
	EXPECT_EQ(renderer.circleCalls, 1);
	EXPECT_EQ(renderer.rectangleCalls, 0);

	// Assert - Verify circle properties were passed correctly
	EXPECT_DOUBLE_EQ(renderer.lastCircleRadius, 25.0);
	EXPECT_EQ(renderer.lastCircleColor.b, Color::blue().b);
}

// Check that SceneManager correctly renders a GameObject with a rectangle
// ShapeRenderer
TEST(ShapeRendererTest, RendersRectangleShapeThroughSceneManager)
{
	// Arrange
	FakeRenderer renderer;
	SceneManager manager;
	manager.setClearColor(Color::black());

	auto scene = std::make_unique<Scene>("RectScene");
	auto rect = std::make_unique<GameObject>();
	rect->getTransform()->setPosition({10.0, 12.0});
	rect->getTransform()->setRotationAngle(33.0);
	rect->getTransform()->setScale({1.0, 1.0});
	rect->addComponent<ShapeRenderer>()
		->setRectangle({80.0, 40.0})
		.setColor(Color::yellow());
	scene->addGameObject(std::move(rect));

	manager.addScene(std::move(scene));
	manager.setActiveScene("RectScene");

	// Act
	RenderQueue queue;
	manager.buildRenderQueue(queue);
	executeRenderQueue(renderer, queue);

	// Assert - Verify rectangle was drawn, not circle
	EXPECT_EQ(renderer.rectangleCalls, 1);
	EXPECT_EQ(renderer.circleCalls, 0);

	// Assert - Verify rectangle properties were passed correctly
	EXPECT_DOUBLE_EQ(renderer.lastRectRotation, 33.0);
	EXPECT_EQ(renderer.lastRectColor.g, Color::yellow().g);
}
