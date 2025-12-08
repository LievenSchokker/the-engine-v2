#include "Animation/Animator.h"

#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationEnums.h"
#include "Animation/AnimationTrack.h"
#include "Animation/SpritesheetAnimationClip.h"
#include "GameObject/GameObject.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>

Animator::Animator()
	: currentClip(nullptr), isPlaying(false), timeScale(1.0f), currentTime(0.0f)
{
}

Animator::~Animator()
{
}

void Animator::onDestroy()
{
	Behaviour::onDestroy();
}

void Animator::play(AnimationClip* clip)
{
	if ( clip == nullptr )
	{
		stop();
		return;
	}

	// Stop current clip if different
	if ( currentClip != clip )
	{
		stop();
	}

	currentClip = clip;
	isPlaying = true;
	currentTime = 0.0f;
}

void Animator::pause()
{
	isPlaying = false;
}

void Animator::resume()
{
	if ( currentClip != nullptr && !isPlaying )
	{
		isPlaying = true;
	}
}

void Animator::stop()
{
	isPlaying = false;
	currentTime = 0.0f;
	// Don't clear currentClip - user might want to Play() again
}

AnimationClip* Animator::getCurrentClip() const
{
	return currentClip;
}

bool Animator::getIsPlaying() const
{
	return isPlaying;
}

float Animator::getTimeScale() const
{
	return timeScale;
}

void Animator::setTimeScale(float scale)
{
	timeScale = scale;
}

float Animator::getCurrentTime() const
{
	return currentTime;
}

void Animator::update(float deltaTime)
{
	// Only update if playing and has a clip
	// GameObject active check is handled by Behaviour's getIsActiveAndEnabled()
	if ( !isPlaying || currentClip == nullptr )
	{
		return;
	}

	// Apply time scale
	float scaledDelta = deltaTime * timeScale;
	currentTime += scaledDelta;

	float clipLength = currentClip->getLength();

	// Handle looping
	if ( currentClip->isLooping() )
	{
		while ( currentTime >= clipLength && clipLength > 0.0f )
		{
			currentTime -= clipLength;
		}
	}
	else
	{
		// Clamp to clip length (only if clip has a valid length)
		if ( clipLength > 0.0f && currentTime >= clipLength )
		{
			currentTime = clipLength;
			isPlaying = false;
		}
		// If clipLength is 0, allow time to advance (for clips with no tracks)
	}

	// Apply tracks at current time
	// Tracks with the same property are applied sequentially (for ping-pong)
	// Tracks with different properties are applied simultaneously
	std::unordered_set<PropertyType> propertyApplied;
	std::map<PropertyType, float> propertyCumulativeOffsets;
	std::map<PropertyType, float> propertyTotalDurations;

	// First pass: compute total durations for each property (needed for Frame
	// looping)
	for ( const auto& track : currentClip->getTracks() )
	{
		float duration = track.getDuration();
		if ( duration > 0.0f )
		{
			propertyTotalDurations[track.getProperty()] += duration;
		}
	}

	// Second pass: apply active tracks
	for ( const auto& track : currentClip->getTracks() )
	{
		float trackDuration = track.getDuration();
		if ( trackDuration <= 0.0f )
		{
			continue;
		}

		PropertyType prop = track.getProperty();

		// Skip if we've already applied a track for this property
		if ( propertyApplied.contains(prop) )
		{
			propertyCumulativeOffsets[prop] += trackDuration;
			continue;
		}

		// Get time offset for this track (cumulative offset before this track)
		float timeOffset = propertyCumulativeOffsets[prop];
		float effectiveTime = currentTime;

		// Handle sprite frame looping: if Frame property and looping enabled,
		// wrap time back to cycle start
		if ( prop == PropertyType::Frame &&
			 clipsWithLoopingFrames.contains(currentClip) )
		{
			float totalFrameDuration =
				propertyTotalDurations[PropertyType::Frame];
			if ( totalFrameDuration > 0.0f )
			{
				effectiveTime = std::fmod(currentTime, totalFrameDuration);
			}
		}

		// Check if effective time is within this track's time range
		if ( effectiveTime >= timeOffset &&
			 effectiveTime < timeOffset + trackDuration )
		{
			// Calculate normalized time for this track (0-1 within track's
			// duration)
			float trackLocalTime = effectiveTime - timeOffset;
			float normalizedTime =
				std::clamp(trackLocalTime / trackDuration, 0.0f, 1.0f);

			// Apply the track
			track.apply(this, normalizedTime);
			propertyApplied.insert(prop);
		}

		// Update cumulative offset for this property
		propertyCumulativeOffsets[prop] += trackDuration;
	}
}

void Animator::addSpritesheetClip(const std::string& name,
								  const SpritesheetAnimationClip& clip)
{
	if ( clip.frames.empty() )
	{
		return;
	}

	// Convert and store the clip
	auto convertedClip = convertSpritesheetClip(clip);
	AnimationClip* clipPtr = convertedClip.get();
	spritesheetClips[name] = std::move(convertedClip);
	clipToNameMap[clipPtr] = name;
}

void Animator::playSpritesheet(const std::string& clipName)
{
	auto it = spritesheetClips.find(clipName);
	if ( it == spritesheetClips.end() )
	{
		return;
	}

	// Play the converted clip
	play(it->second.get());
}

std::string Animator::getCurrentSpritesheetClipName() const
{
	if ( currentClip == nullptr )
	{
		return "";
	}

	// O(1) lookup using reverse mapping
	auto it = clipToNameMap.find(currentClip);
	if ( it != clipToNameMap.end() )
	{
		return it->second;
	}
	return "";
}

bool Animator::hasSpritesheetClip(const std::string& name) const
{
	return spritesheetClips.find(name) != spritesheetClips.end();
}

std::unique_ptr<AnimationClip> Animator::convertSpritesheetClip(
	const SpritesheetAnimationClip& clip)
{
	// Create AnimationClip with same name and loop setting
	auto animationClip = std::make_unique<AnimationClip>(clip.name, clip.loop);

	// Add frame tracks (single cycle for standalone clips)
	addFrameTracksToClip(animationClip.get(), clip, 1);

	return animationClip;
}

void Animator::addFrameTracksToClip(
	AnimationClip* clip, const SpritesheetAnimationClip& spritesheetClip,
	int repeatCount)
{
	if ( clip == nullptr || spritesheetClip.frames.empty() || repeatCount <= 0 )
	{
		return;
	}

	AnimationCurve curve(EasingType::Linear);

	// If only one frame, create tracks that repeat
	if ( spritesheetClip.frames.size() == 1 )
	{
		for ( int i = 0; i < repeatCount; ++i )
		{
			AnimationTrack track(TargetType::Sprite, PropertyType::Frame,
								 spritesheetClip.frameDuration, false,
								 spritesheetClip.frames[0],
								 spritesheetClip.frames[0], curve);
			clip->addTrack(track);
		}
	}
	else
	{
		// Create sequential tracks for each frame transition
		// For frames {f0, f1, f2, f3}, create tracks: f0->f1, f1->f2, f2->f3
		// Repeat this sequence repeatCount times
		for ( int cycle = 0; cycle < repeatCount; ++cycle )
		{
			for ( size_t i = 0; i < spritesheetClip.frames.size() - 1; ++i )
			{
				int fromFrame = spritesheetClip.frames[i];
				int toFrame = spritesheetClip.frames[i + 1];

				AnimationTrack track(TargetType::Sprite, PropertyType::Frame,
									 spritesheetClip.frameDuration, false,
									 fromFrame, toFrame, curve);
				clip->addTrack(track);
			}
		}
	}
}

void Animator::addSpritesheetTracksToClip(
	AnimationClip* targetClip, const SpritesheetAnimationClip& spritesheetClip)
{
	if ( targetClip == nullptr || spritesheetClip.frames.empty() )
	{
		return;
	}

	// Always add only one cycle - update() will handle looping automatically
	addFrameTracksToClip(targetClip, spritesheetClip, 1);

	// Store loop flag if sprite animation should loop
	// Cycle duration will be computed automatically from tracks in update()
	if ( spritesheetClip.loop )
	{
		clipsWithLoopingFrames.insert(targetClip);
	}
}
