#include "Component/SpriteComponent.h"

#include "Component/Transform.h"
#include "Math/Vector2Utils.h"

#include <algorithm>

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
	if ( !hasCustomSize )
	{
		renderSize = {static_cast<float>(spritesheetDef.frameWidth),
					  static_cast<float>(spritesheetDef.frameHeight)};
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
	if ( hasCustomSize )
	{
		return renderSize;
	}
	return {static_cast<float>(spritesheetDef.frameWidth),
			static_cast<float>(spritesheetDef.frameHeight)};
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

std::optional<SpriteRenderCommand> SpriteComponent::buildRenderCommand() const
{
	if ( sprite == nullptr || !sprite->isLoaded() || getFrameCount() == 0 )
	{
		return std::nullopt;
	}

	const Transform* transform = getTransform();
	if ( transform == nullptr )
	{
		return std::nullopt;
	}

	Rect srcRect = calculateSourceRect();
	if ( srcRect.isEmpty() )
	{
		return std::nullopt;
	}

	const Vector2 position = transform->getPosition() + offset;
	const double rotation = transform->getRotationAngle();
	const Vector2 scale = Vector2Utils::sanitizeScale(transform->getScale());
	const Vector2 size = getSize();

	SpriteRenderCommand command;
	command.sprite = sprite;
	command.srcRect = srcRect;
	command.position = position;
	command.size = size;
	command.rotationDegrees = rotation;
	command.scale = scale;
	command.tint = tint;
	command.flipX = flipX;
	command.flipY = flipY;

	return command;
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
