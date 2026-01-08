#include "Component/Camera.h"

Camera::Camera(float zoom, Vector2 offset, float viewWidth, float viewHeight)
	: zoom(zoom)
	, offset(offset)
	, viewWidth(viewWidth)
	, viewHeight(viewHeight)
{
}

float Camera::getZoom() const
{
	return zoom;
}

void Camera::setZoom(float cameraZoom)
{
	zoom = std::fabsf(cameraZoom);
}

Vector2 Camera::getOffset() const
{
	return offset;
}

void Camera::setOffset(Vector2 newOffset)
{
	offset = newOffset;
}

float Camera::getViewWidth() const
{
	return viewWidth;
}

float Camera::getViewHeight() const
{
	return viewHeight;
}


void Camera::serialize(WriteArchive& archive) const
{
    archive.process(zoom);
    archive.process(offset.x);
    archive.process(offset.y);
    archive.process(viewHeight);
	archive.process(viewWidth);
}

void Camera::deserialize(ReadArchive& archive)
{
    archive.process(zoom);
    archive.process(offset.x);
    archive.process(offset.y);
    archive.process(viewHeight);
	archive.process(viewWidth);
}