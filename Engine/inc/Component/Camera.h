// Camera.h
#pragma once

#include "BaseComponentTypes/Component.h"
#include "Math/Vector2.h"

/**
 * @class Camera
 * @brief Component that defines a view into the game world.
 *
 * The Camera transforms world-space coordinates into view-space for rendering.
 * It supports zooming, offset adjustments, and viewport positioning.
 *
 * The camera's position in the world is determined by its GameObject's Transform.
 * The offset provides additional displacement on top of the transform position,
 * useful for screen shake effects or look-ahead behavior.
 *
 */
class Camera final : public Component
{
public:
    /**
     * @brief Constructs a Camera with the specified parameters.
     * @param zoom Zoom level where 1 is normal
     * @param offset Additional offset from the transform position in world units
     * @param viewWidth Width of the camera's view in pixels
     * @param viewHeight Height of the camera's view in pixels
     */
    Camera(float zoom, Vector2 offset, float viewWidth, float viewHeight);

    ~Camera() override = default;

    /**
     * @brief Gets the current zoom level.
     * @return Zoom level where 1 is normal
     */
    float getZoom() const;

    /**
     * @brief Sets the zoom level.
     * @param zoom Zoom level where 1 is normal
     */
    void setZoom(float zoom);

    /**
     * @brief Gets the camera offset.
     * @return Offset vector added to the transform position
     */
    Vector2 getOffset() const;

    /**
     * @brief Sets the camera offset.
     *
     * The offset is added to the GameObject's transform position to determine
     * where the camera looks.
     *
     * @param offset Offset in world units
     */
    void setOffset(Vector2 offset);

    /**
     * @brief Gets the view width.
     * @return Width of the camera's view in pixels
     */
    float getViewWidth() const;

    /**
     * @brief Gets the view height.
     * @return Height of the camera's view in pixels
     */
    float getViewHeight() const;

    /**
     * @brief Gets the viewport X position on screen.
     * @return X coordinate of the viewport's top-left corner in screen pixels
     */
    float getViewportX() const;

    /**
     * @brief Gets the viewport Y position on screen.
     * @return Y coordinate of the viewport's top-left corner in screen pixels
     */
    float getViewportY() const;

    /**
     * @brief Sets the viewport position on screen.
     *
     * The viewport determines where on the screen this camera renders its output.
     * For split-screen setups:
     * - Left camera: setViewport(0, 0)
     * - Right camera: setViewport(screenWidth / 2, 0)
     *
     * @param x X coordinate of the viewport's top-left corner
     * @param y Y coordinate of the viewport's top-left corner
     */
    void setViewport(float x, float y);

private:
    float zoom;
    Vector2 offset;
    float viewWidth;
    float viewHeight;
    float viewportX;
    float viewportY;
};