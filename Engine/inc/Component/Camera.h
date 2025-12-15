#pragma once


#include "BaseComponentTypes/Component.h"
#include "Math/Vector2.h"

class Camera final : public Component
{
public:
	Camera(float zoom, Vector2 offset, float viewWidth, float viewHeight)
		: zoom(zoom)
		, offset(offset)
		, viewWidth(viewWidth)
		, viewHeight(viewHeight)
		, viewportX(0)
		, viewportY(0)
	{}

	~Camera() override = default;

	float getZoom() const { return zoom; }
	void setZoom(float z) { zoom = z; }

	Vector2 getOffset() const { return offset; }
	void setOffset(Vector2 o) { offset = o; }

	float getViewWidth() const { return viewWidth; }
	float getViewHeight() const { return viewHeight; }

	// Viewport = where on screen this camera renders
	float getViewportX() const { return viewportX; }
	float getViewportY() const { return viewportY; }
	void setViewport(float x, float y) { viewportX = x; viewportY = y; }

private:
	float zoom;
	Vector2 offset;
	float viewWidth;
	float viewHeight;
	float viewportX;
	float viewportY;
};