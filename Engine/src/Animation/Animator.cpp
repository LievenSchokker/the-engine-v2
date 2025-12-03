#include "Animation/Animator.h"

#include "Animation/AnimationCurve.h"
#include "Animation/AnimationSystem.h"
#include "GameObject/GameObject.h"

#include <algorithm>
#include <map>

Animator::Animator()
	: currentClip(nullptr),
	  isPlaying(false),
	  timeScale(1.0f),
	  currentTime(0.0f),
	  animationSystem(nullptr),
	  isRegistered(false)
{
}

Animator::~Animator()
{
	unregisterFromSystem();
}

void Animator::onDestroy()
{
	unregisterFromSystem();
	Component::onDestroy();
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

	registerWithSystem();
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
		registerWithSystem();
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

void Animator::setAnimationSystem(AnimationSystem* system)
{
	if ( animationSystem != system )
	{
		unregisterFromSystem();
		animationSystem = system;
		if ( isPlaying && animationSystem != nullptr )
		{
			registerWithSystem();
		}
	}
}

void Animator::clearAnimationSystem()
{
	// Clear the reference without trying to unregister
	// This is called when AnimationSystem is being destroyed
	animationSystem = nullptr;
	isRegistered = false;
}

void Animator::update(float deltaTime)
{
	if ( !isPlaying || currentClip == nullptr || gameObject == nullptr ||
		 !gameObject->getIsActive() )
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
	std::map<PropertyType, bool> propertyApplied;

	// Iterate through tracks and apply the active one for each property
	for ( const auto& track : currentClip->getTracks() )
	{
		float trackDuration = track.getDuration();
		if ( trackDuration <= 0.0f )
		{
			continue;
		}

		PropertyType prop = track.getProperty();

		// Skip if we've already applied a track for this property
		if ( propertyApplied.find(prop) != propertyApplied.end() &&
			 propertyApplied[prop] )
		{
			continue;
		}

		// Calculate time offset for this track by summing durations of
		// previous tracks with the same property
		float timeOffset = 0.0f;
		for ( const auto& prevTrack : currentClip->getTracks() )
		{
			if ( prevTrack.getProperty() == prop )
			{
				// If we've reached the current track, stop
				if ( &prevTrack == &track )
				{
					break;
				}
				// Otherwise, add this track's duration to the offset
				timeOffset += prevTrack.getDuration();
			}
		}

		// Check if current time is within this track's time range
		if ( currentTime >= timeOffset &&
			 currentTime < timeOffset + trackDuration )
		{
			// Calculate normalized time for this track (0-1 within track's
			// duration)
			float trackLocalTime = currentTime - timeOffset;
			float normalizedTime =
				std::clamp(trackLocalTime / trackDuration, 0.0f, 1.0f);

			// Apply the track
			track.apply(this, normalizedTime);
			propertyApplied[prop] = true;
		}
	}
}

void Animator::registerWithSystem()
{
	if ( animationSystem != nullptr && !isRegistered )
	{
		animationSystem->registerAnimator(this);
		isRegistered = true;
	}
}

void Animator::unregisterFromSystem()
{
	if ( animationSystem != nullptr && isRegistered )
	{
		animationSystem->unregisterAnimator(this);
		isRegistered = false;
	}
}
