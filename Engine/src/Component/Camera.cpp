// Camera.cpp
#include "Component/Camera.h"

Camera::Camera(float zoom, Vector2 offset, float viewWidth, float viewHeight)
	: zoom(zoom)
	, offset(offset)
	, viewWidth(viewWidth)
	, viewHeight(viewHeight)
	, viewportX(0)
	, viewportY(0)
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

float Camera::getViewportX() const
{
	return viewportX;
}

float Camera::getViewportY() const
{
	return viewportY;
}

void Camera::setViewport(float x, float y)
{
	viewportX = x;
	viewportY = y;
}