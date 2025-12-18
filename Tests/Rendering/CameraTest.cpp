#include <gtest/gtest.h>

#include "Component/Camera.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/ViewAdapters/WorldToCameraSpaceAdapter.h"

class CameraTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        cameraObject = std::make_unique<GameObject>();
        cameraObject->getTransform()->setPosition({0, 0});
    }

    std::unique_ptr<GameObject> cameraObject;
};


TEST_F(CameraTest, SetOffset_UpdatesOffset)
{
    auto* camera = cameraObject->addComponent<Camera>(1.0f, Vector2{0, 0}, 800, 600);

    camera->setOffset(Vector2{100, 50});
    EXPECT_FLOAT_EQ(camera->getOffset().x, 100.0f);
    EXPECT_FLOAT_EQ(camera->getOffset().y, 50.0f);
}

TEST_F(CameraTest, SetViewport_UpdatesViewportPosition)
{
    auto* camera = cameraObject->addComponent<Camera>(1.0f, Vector2{0, 0}, 800, 600);

    camera->setViewport(400, 0);
    EXPECT_FLOAT_EQ(camera->getViewportX(), 400.0f);
    EXPECT_FLOAT_EQ(camera->getViewportY(), 0.0f);
}



