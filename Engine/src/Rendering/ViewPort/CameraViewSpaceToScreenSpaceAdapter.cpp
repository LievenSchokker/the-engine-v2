#include "Rendering/viewport/CameraViewSpaceToScreenSpaceAdapter.h"

#include "Component/Transform.h"
#include <algorithm>

RenderCommand CameraViewSpaceToScreenSpaceAdapter::Transform(
    const Camera& camera, const WindowOptions& window, const RenderCommand& command) {

    RenderCommand result = command;

    const float scaleX = window.width / camera.getViewWidth();
    const float scaleY = window.height / camera.getViewHeight();

    const float scale = std::min(scaleX, scaleY);

    const float viewportWidth = camera.getViewWidth() * scale;
    const float viewportHeight = camera.getViewHeight() * scale;

    const float offsetX = (window.width - viewportWidth) / 2.0f;
    const float offsetY = (window.height - viewportHeight) / 2.0f;

    const float centerX = offsetX + viewportWidth / 2.0f;
    const float centerY = offsetY + viewportHeight / 2.0f;

    result.position.x = command.position.x * scale + centerX;
    result.position.y = command.position.y * scale + centerY;

    result.size.x = command.size.x * scale;
    result.size.y = command.size.y * scale;
    result.radius = command.radius * scale;

    return result;
}

Vector2 CameraViewSpaceToScreenSpaceAdapter::findOptimalResolution(const Camera& camera, const WindowOptions& window)
{
    const float scaleX = window.width / camera.getViewWidth();
    const float scaleY = window.height / camera.getViewHeight();
    const float scale = std::min(scaleX, scaleY);

    return Vector2{
        camera.getViewWidth() * scale,
        camera.getViewHeight() * scale
    };
}