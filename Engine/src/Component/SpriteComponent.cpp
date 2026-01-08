#include "Component/SpriteComponent.h"

#include "Component/Transform.h"
#include "Math/Vector2Utils.h"
#include "Rendering/RenderCommandType.h"
#include "Rendering/RenderQueue/IRenderQueueWriter.h"

#include <algorithm>
#include <numeric>

void SpriteComponent::setSprite(IImage* image, SpritesheetDefinition def)
{
	sprite = image;
	spritesheetDef = def;
	currentFrame = 0;

	// Validate spritesheet definition
	if ( spritesheetDef.rows <= 0 || spritesheetDef.columns <= 0 ||
		 spritesheetDef.frameWidth <= 0 || spritesheetDef.frameHeight <= 0 )
	{
		spritesheetDef = {0, 0, 0, 0};
		sprite = nullptr;
		return;
	}

	// Clamp current frame to valid range
	if ( currentFrame >= getFrameCount() )
	{
		currentFrame = std::max(0, getFrameCount() - 1);
	}

	// Set default render size to frame size if not custom
	if (!hasCustomSize)
	{
		int gcd = std::gcd(spritesheetDef.frameWidth, spritesheetDef.frameHeight);
		renderSize = {
			static_cast<float>(spritesheetDef.frameWidth / gcd),
			static_cast<float>(spritesheetDef.frameHeight / gcd)
		};
	}
}

void SpriteComponent::setFrame(int frameIndex)
{
	currentFrame = std::clamp(frameIndex, 0, std::max(0, getFrameCount() - 1));
}

int SpriteComponent::getFrame() const
{
	return currentFrame;
}

int SpriteComponent::getFrameCount() const
{
	return spritesheetDef.getTotalFrames();
}

const SpritesheetDefinition& SpriteComponent::getSpritesheetDefinition() const
{
	return spritesheetDef;
}

IImage* SpriteComponent::getSprite() const
{
	return sprite;
}

void SpriteComponent::setSize(Vector2 size)
{
	renderSize = size;
	hasCustomSize = true;
}

Vector2 SpriteComponent::getSize() const
{
	return renderSize;
}

void SpriteComponent::setTint(const Color& newTint)
{
	tint = newTint;
}

Color SpriteComponent::getTint() const
{
	return tint;
}

void SpriteComponent::setOffset(Vector2 newOffset)
{
	offset = newOffset;
}

Vector2 SpriteComponent::getOffset() const
{
	return offset;
}

void SpriteComponent::setFlipX(bool newFlipX)
{
	flipX = newFlipX;
}

bool SpriteComponent::getFlipX() const
{
	return flipX;
}

void SpriteComponent::setFlipY(bool newFlipY)
{
	flipY = newFlipY;
}

bool SpriteComponent::getFlipY() const
{
	return flipY;
}

void SpriteComponent::fillRenderQueue(IRenderQueueWriter& queue) const
{
	if ( sprite == nullptr || !sprite->isLoaded() || getFrameCount() == 0 )
	{
		return;
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr )
	{
		return;
	}

	Rect srcRect = calculateSourceRect();
	if ( srcRect.isEmpty() )
	{
		return;
	}

	const Vector2 position = transform->getPosition() + offset;
	const double rotation = transform->getRotationAngle();
	const Vector2 scale = Vector2Utils::sanitizeScale(transform->getScale());
	const Vector2 size = getSize();

	RenderCommand command;
	command.type = RenderCommandType::Sprite;
	command.sprite = sprite;
	command.srcRect = srcRect;
	command.position = position;
	command.size = size;
	command.rotationDegrees = rotation;
	command.scale = scale;
	command.tint = tint;
	command.flipX = flipX;
	command.flipY = flipY;
	command.layer = 0;		   // Can be extended later if needed
	command.orderInLayer = 0;  // Can be extended later if needed

	queue.push(command);
}

Rect SpriteComponent::calculateSourceRect() const
{
	if ( getFrameCount() == 0 || currentFrame < 0 ||
		 currentFrame >= getFrameCount() )
	{
		return {0, 0, 0, 0};
	}

	// Calculate row and column from frame index (row-major order)
	int row = currentFrame / spritesheetDef.columns;
	int col = currentFrame % spritesheetDef.columns;

	// Calculate source rectangle
	Rect srcRect;
	srcRect.x = col * spritesheetDef.frameWidth;
	srcRect.y = row * spritesheetDef.frameHeight;
	srcRect.w = spritesheetDef.frameWidth;
	srcRect.h = spritesheetDef.frameHeight;

	return srcRect;
}
