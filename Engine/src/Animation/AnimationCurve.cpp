#include "Animation/AnimationCurve.h"

#include <algorithm>
#include <cmath>

AnimationCurve::AnimationCurve(EasingType type) : type(type)
{
}

float AnimationCurve::evaluate01(float x) const
{
	// Clamp x to [0, 1]
	x = std::clamp(x, 0.0f, 1.0f);

	switch ( type )
	{
		case EasingType::Linear:
			return x;

		case EasingType::EaseInQuad:
			return x * x;

		case EasingType::EaseOutQuad:
			return 1.0f - (1.0f - x) * (1.0f - x);

		case EasingType::EaseInOutQuad:
		{
			if ( x < 0.5f )
			{
				return 2.0f * x * x;
			}
			else
			{
				return 1.0f - 2.0f * (1.0f - x) * (1.0f - x);
			}
		}

		case EasingType::EaseInCubic:
			return x * x * x;

		case EasingType::EaseOutCubic:
		{
			float t = 1.0f - x;
			return 1.0f - t * t * t;
		}

		case EasingType::EaseInOutCubic:
		{
			if ( x < 0.5f )
			{
				return 4.0f * x * x * x;
			}
			else
			{
				float t = 2.0f * x - 2.0f;
				return 1.0f + t * t * t / 2.0f;
			}
		}

		default:
			return x;
	}
}

EasingType AnimationCurve::getType() const
{
	return type;
}

void AnimationCurve::setType(EasingType newType)
{
	type = newType;
}

void AnimationCurve::serialize(CerealWriteArchive& archive) const
{
    auto easingType = static_cast<uint8_t>(type);
    archive.process(easingType);
}

void AnimationCurve::deserialize(ReadArchive& archive)
{
    uint8_t easingType;
    archive.process(easingType);
    type = static_cast<EasingType>(easingType);
}