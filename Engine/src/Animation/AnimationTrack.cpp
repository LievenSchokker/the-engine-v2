#include "Animation/AnimationTrack.h"

#include "Animation/Animator.h"
#include "Component/SpriteComponent.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"

#include <algorithm>
#include <cmath>

AnimationTrack::AnimationTrack(TargetType target, PropertyType property,
							   float duration, bool relative,
							   std::variant<Vector2, float, int> fromValue,
							   std::variant<Vector2, float, int> toValue,
							   AnimationCurve curve)
	: target(target),
	  property(property),
	  duration(duration),
	  relative(relative),
	  fromValue(fromValue),
	  toValue(toValue),
	  curve(curve)
{
}

std::variant<Vector2, float, int> AnimationTrack::sample(float t) const
{
	// Clamp t to [0, 1]
	t = std::clamp(t, 0.0f, 1.0f);

	// Evaluate curve
	float easedT = curve.evaluate01(t);

	// Interpolate based on value type
	if ( std::holds_alternative<Vector2>(fromValue) &&
		 std::holds_alternative<Vector2>(toValue) )
	{
		Vector2 from = std::get<Vector2>(fromValue);
		Vector2 to = std::get<Vector2>(toValue);

		Vector2 result = Vector2::lerp(from, to, easedT);

		return result;
	}
	else if ( std::holds_alternative<float>(fromValue) &&
			  std::holds_alternative<float>(toValue) )
	{
		float from = std::get<float>(fromValue);
		float to = std::get<float>(toValue);

		float result = from + (to - from) * easedT;
		return result;
	}
	else if ( std::holds_alternative<int>(fromValue) &&
			  std::holds_alternative<int>(toValue) )
	{
		// For frame animation, use discrete frame changes (round to nearest
		// integer)
		int from = std::get<int>(fromValue);
		int to = std::get<int>(toValue);

		float interpolated =
			static_cast<float>(from) +
			(static_cast<float>(to) - static_cast<float>(from)) * easedT;
		int result = static_cast<int>(std::round(interpolated));
		return result;
	}

	// Fallback (should not happen if track is constructed correctly)
	return fromValue;
}

void AnimationTrack::apply(Animator* animator, float normalizedTime) const
{
	if ( animator == nullptr )
	{
		return;
	}

	GameObject* gameObject = animator->getGameObject();
	if ( gameObject == nullptr )
	{
		return;
	}

	// Evaluate easing curve
	float easedT = curve.evaluate01(normalizedTime);

	// Handle Sprite target
	if ( target == TargetType::Sprite )
	{
		SpriteComponent* sprite = gameObject->getComponent<SpriteComponent>();
		if ( sprite == nullptr )
		{
			return;
		}

		if ( property == PropertyType::Frame )
		{
			if ( std::holds_alternative<int>(fromValue) &&
				 std::holds_alternative<int>(toValue) )
			{
				int fromVal = std::get<int>(fromValue);
				int toVal = std::get<int>(toValue);

				// Clamp to valid frame range
				int maxFrame = std::max(0, sprite->getFrameCount() - 1);
				fromVal = std::clamp(fromVal, 0, maxFrame);
				toVal = std::clamp(toVal, 0, maxFrame);

				if ( relative )
				{
					// When relative: start from (current + fromValue), end at
					// toValue
					int currentFrame = sprite->getFrame();
					int startFrame =
						std::clamp(currentFrame + fromVal, 0, maxFrame);
					// Calculate frame based on eased time
					float frameFloat = static_cast<float>(startFrame) +
									   (static_cast<float>(toVal) -
										static_cast<float>(startFrame)) *
										   easedT;
					int finalFrame = static_cast<int>(
						std::floor(frameFloat + 0.5f));	 // Round to nearest
					finalFrame = std::clamp(finalFrame, 0, maxFrame);
					sprite->setFrame(finalFrame);
				}
				else
				{
					// Absolute: calculate discrete frame index directly
					// For sprite frames, we want discrete steps, not
					// interpolation
					int totalFrames = std::abs(toVal - fromVal) + 1;
					int frameIndex =
						fromVal + static_cast<int>(easedT * (totalFrames - 1));
					frameIndex = std::clamp(frameIndex, fromVal, toVal);
					sprite->setFrame(frameIndex);
				}
			}
		}
		else if ( property == PropertyType::Offset )
		{
			if ( std::holds_alternative<Vector2>(fromValue) &&
				 std::holds_alternative<Vector2>(toValue) )
			{
				Vector2 fromVal = std::get<Vector2>(fromValue);
				Vector2 toVal = std::get<Vector2>(toValue);

				if ( relative )
				{
					Vector2 currentOffset = sprite->getOffset();
					Vector2 startOffset(currentOffset.x() + fromVal.x(),
										currentOffset.y() + fromVal.y());
					Vector2 finalOffset =
						Vector2::lerp(startOffset, toVal, easedT);
					sprite->setOffset(finalOffset);
				}
				else
				{
					Vector2 finalOffset = Vector2::lerp(fromVal, toVal, easedT);
					sprite->setOffset(finalOffset);
				}
			}
		}
		else if ( property == PropertyType::FlipX )
		{
			if ( std::holds_alternative<int>(fromValue) &&
				 std::holds_alternative<int>(toValue) )
			{
				int fromVal = std::get<int>(fromValue);
				int toVal = std::get<int>(toValue);

				// Interpolate between 0 and 1, then round to nearest boolean
				float flipFloat =
					static_cast<float>(fromVal) +
					(static_cast<float>(toVal) - static_cast<float>(fromVal)) *
						easedT;
				bool flipValue = static_cast<int>(std::round(flipFloat)) != 0;
				sprite->setFlipX(flipValue);
			}
		}
		else if ( property == PropertyType::FlipY )
		{
			if ( std::holds_alternative<int>(fromValue) &&
				 std::holds_alternative<int>(toValue) )
			{
				int fromVal = std::get<int>(fromValue);
				int toVal = std::get<int>(toValue);

				// Interpolate between 0 and 1, then round to nearest boolean
				float flipFloat =
					static_cast<float>(fromVal) +
					(static_cast<float>(toVal) - static_cast<float>(fromVal)) *
						easedT;
				bool flipValue = static_cast<int>(std::round(flipFloat)) != 0;
				sprite->setFlipY(flipValue);
			}
		}
		return;
	}

	// Handle Transform target (existing code)
	Transform* transform = gameObject->getTransform();
	if ( transform == nullptr )
	{
		return;
	}

	if ( property == PropertyType::Position )
	{
		if ( std::holds_alternative<Vector2>(fromValue) &&
			 std::holds_alternative<Vector2>(toValue) )
		{
			Vector2 fromVal = std::get<Vector2>(fromValue);
			Vector2 toVal = std::get<Vector2>(toValue);

			if ( relative )
			{
				// When relative: start from (current + fromValue), end at
				// toValue (absolute)
				Vector2 currentPos = transform->getPosition();
				Vector2 startPos(currentPos.x() + fromVal.x(),
								 currentPos.y() + fromVal.y());
				Vector2 finalPos = Vector2::lerp(startPos, toVal, easedT);
				transform->setPosition(finalPos);
			}
			else
			{
				// Absolute: interpolate directly between fromValue and toValue
				Vector2 finalPos = Vector2::lerp(fromVal, toVal, easedT);
				transform->setPosition(finalPos);
			}
		}
	}
	else if ( property == PropertyType::Rotation )
	{
		if ( std::holds_alternative<float>(fromValue) &&
			 std::holds_alternative<float>(toValue) )
		{
			float fromVal = std::get<float>(fromValue);
			float toVal = std::get<float>(toValue);

			if ( relative )
			{
				// When relative: start from (current + fromValue), end at
				// toValue (absolute)
				double currentRot = transform->getRotationAngle();
				float startRot = static_cast<float>(currentRot) + fromVal;
				float finalRot = startRot + (toVal - startRot) * easedT;
				transform->setRotationAngle(static_cast<double>(finalRot));
			}
			else
			{
				// Absolute: interpolate directly between fromValue and toValue
				float finalRot = fromVal + (toVal - fromVal) * easedT;
				transform->setRotationAngle(static_cast<double>(finalRot));
			}
		}
	}
	else if ( property == PropertyType::Scale )
	{
		if ( std::holds_alternative<Vector2>(fromValue) &&
			 std::holds_alternative<Vector2>(toValue) )
		{
			Vector2 fromVal = std::get<Vector2>(fromValue);
			Vector2 toVal = std::get<Vector2>(toValue);

			if ( relative )
			{
				// When relative: start from (current + fromValue), end at
				// toValue (absolute)
				Vector2 currentScale = transform->getScale();
				Vector2 startScale(currentScale.x() + fromVal.x(),
								   currentScale.y() + fromVal.y());
				Vector2 finalScale = Vector2::lerp(startScale, toVal, easedT);
				transform->setScale(finalScale);
			}
			else
			{
				// Absolute: interpolate directly between fromValue and toValue
				Vector2 finalScale = Vector2::lerp(fromVal, toVal, easedT);
				transform->setScale(finalScale);
			}
		}
	}
}

TargetType AnimationTrack::getTarget() const
{
	return target;
}

PropertyType AnimationTrack::getProperty() const
{
	return property;
}

float AnimationTrack::getDuration() const
{
	return duration;
}

bool AnimationTrack::isRelative() const
{
	return relative;
}

const std::variant<Vector2, float, int>& AnimationTrack::getFromValue() const
{
	return fromValue;
}

const std::variant<Vector2, float, int>& AnimationTrack::getToValue() const
{
	return toValue;
}

const AnimationCurve& AnimationTrack::getCurve() const
{
	return curve;
}
