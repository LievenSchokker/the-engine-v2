#pragma once

#include "Animation/SpritesheetDefinition.h"
#include "Assets/IImage.h"
#include "BaseComponentTypes/RenderComponentBase.h"
#include "Component/BaseComponentTypes/RenderComponent.h"
#include "Math/Vector2.h"
#include "Rendering/Color.h"
#include "Rendering/Rect.h"

/**
 * @class SpriteComponent
 * @brief Component that renders a sprite from a spritesheet.
 *
 * SpriteComponent displays a single frame from a spritesheet image based on
 * a grid layout definition. The frame index can be animated using the
 * animation system.
 */
class SpriteComponent: public RenderComponentBase<SpriteComponent>
{
   public:
	SpriteComponent() = default;
    static constexpr const char* name() { return "Sprite"; }
	/**
	 * @brief Sets the spritesheet image and grid definition.
	 *
	 * @param image Pointer to the IImage asset containing the spritesheet
	 * @param def SpritesheetDefinition describing the grid layout
	 */
	void setSprite(IImage* image, SpritesheetDefinition def);

	/**
	 * @brief Sets the current frame index to display.
	 *
	 * @param frameIndex Frame index (0-based, clamped to valid range)
	 */
	void setFrame(int frameIndex);

	/**
	 * @brief Gets the current frame index.
	 * @return Current frame index
	 */
	int getFrame() const;

	/**
	 * @brief Gets the total number of frames in the spritesheet.
	 * @return Total frames, or 0 if no spritesheet is set
	 */
	int getFrameCount() const;

	/**
	 * @brief Gets the spritesheet definition.
	 * @return Reference to the SpritesheetDefinition
	 */
	const SpritesheetDefinition& getSpritesheetDefinition() const;

	/**
	 * @brief Gets the sprite image.
	 * @return Pointer to IImage, or nullptr if not set
	 */
	IImage* getSprite() const;

	/**
	 * @brief Sets the render size (overrides frame size).
	 *
	 * If not set, uses the frame size from SpritesheetDefinition.
	 *
	 * @param size Render size in world units
	 */
	void setSize(Vector2 size);

	/**
	 * @brief Gets the render size.
	 * @return Render size, or frame size if not explicitly set
	 */
	Vector2 getSize() const;

	/**
	 * @brief Sets the color tint.
	 * @param tint Color tint (defaults to white for no tinting)
	 */
	void setTint(const Color& tint);

	/**
	 * @brief Gets the color tint.
	 * @return Color tint
	 */
	Color getTint() const;

	/**
	 * @brief Sets the render offset.
	 *
	 * Offset is added to the sprite position when rendering.
	 *
	 * @param offset Offset in world units
	 */
	void setOffset(Vector2 offset);

	/**
	 * @brief Gets the render offset.
	 * @return Offset in world units
	 */
	Vector2 getOffset() const;

	/**
	 * @brief Sets horizontal flip state.
	 *
	 * @param flipX true to flip horizontally, false otherwise
	 */
	void setFlipX(bool flipX);

	/**
	 * @brief Gets horizontal flip state.
	 * @return true if flipped horizontally, false otherwise
	 */
	bool getFlipX() const;

	/**
	 * @brief Sets vertical flip state.
	 *
	 * @param flipY true to flip vertically, false otherwise
	 */
	void setFlipY(bool flipY);

	/**
	 * @brief Gets vertical flip state.
	 * @return true if flipped vertically, false otherwise
	 */
	bool getFlipY() const;

	/**
	 * @brief Fills the render queue with a sprite render command.
	 *
	 * @param queue The render queue writer to push commands to
	 */
	void fillRenderQueue(IRenderQueueWriter& queue) const override;

   private:
	IImage* sprite = nullptr;
	SpritesheetDefinition spritesheetDef{0, 0, 0, 0};
	int currentFrame = 0;
	Vector2 renderSize{0.0, 0.0};
	bool hasCustomSize = false;
	Color tint = Color::white();
	Vector2 offset{0.0, 0.0};
	bool flipX = false;
	bool flipY = false;

	/**
	 * @brief Calculates the source rectangle for the current frame.
	 * @return Rect for the current frame, or empty rect if invalid
	 */
	Rect calculateSourceRect() const;
};
