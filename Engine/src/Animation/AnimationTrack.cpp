#include "Animation/AnimationTrack.h"

#include "Animation/Animator.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"

#include <algorithm>

AnimationTrack::AnimationTrack(TargetType target, PropertyType property,
							   float duration, bool relative,
							   std::variant<Vector2, float> fromValue,
							   std::variant<Vector2, float> toValue,
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

std::variant<Vector2, float> AnimationTrack::sample(float t) const
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

	Transform* transform = gameObject->getTransform();
	if ( transform == nullptr )
	{
		return;
	}

	// Evaluate easing curve
	float easedT = curve.evaluate01(normalizedTime);

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
				Vector2 startPos(currentPos.x + fromVal.x,
								 currentPos.y + fromVal.y);
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
				Vector2 startScale(currentScale.x + fromVal.x,
								   currentScale.y + fromVal.y);
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

const std::variant<Vector2, float>& AnimationTrack::getFromValue() const
{
	return fromValue;
}

const std::variant<Vector2, float>& AnimationTrack::getToValue() const
{
	return toValue;
}

const AnimationCurve& AnimationTrack::getCurve() const
{
	return curve;
}
