#include <gtest/gtest.h>

#include "Component/Camera.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/ViewAdapters/WorldToCameraSpaceAdapter.h"

class CameraTransformTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		cameraObject = std::make_unique<GameObject>();
		cameraObject->getTransform()->setPosition({0, 0});
	}

	static RenderCommand createCircleCommand(float x, float y, float radius)
	{
		RenderCommand cmd;
		cmd.type = RenderCommandType::Circle;
		cmd.position = {x, y};
		cmd.radius = radius;
		return cmd;
	}

	std::unique_ptr<GameObject> cameraObject;
};

TEST_F(CameraTransformTest, Transform_ObjectInView_ReturnsCommand)
{
	const auto* camera = cameraObject->addComponent<Camera>(1.0f, Vector2{0, 0}, 800, 600);
	const RenderCommand command = createCircleCommand(0, 0, 10);

	const auto result = WorldToCameraSpaceAdapter::Transform(*camera, command);

	EXPECT_TRUE(result.has_value());
}

TEST_F(CameraTransformTest, Transform_ObjectOutsideViewLeft_ReturnsCulled)
{
	const auto* camera = cameraObject->addComponent<Camera>(1.0f, Vector2{0, 0}, 800, 600);
	const RenderCommand cmd = createCircleCommand(-500, 0, 10);

	const auto result = WorldToCameraSpaceAdapter::Transform(*camera, cmd);

	EXPECT_FALSE(result.has_value());
}

