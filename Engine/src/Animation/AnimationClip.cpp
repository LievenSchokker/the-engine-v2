#include "Animation/AnimationClip.h"

#include "Animation/AnimationCurve.h"

#include <algorithm>
#include <map>

AnimationClip::AnimationClip(const std::string& name, bool loop)
	: name(name), length(0.0f), loop(loop)
{
}

const std::string& AnimationClip::getName() const
{
	return name;
}

void AnimationClip::setName(const std::string& newName)
{
	name = newName;
}

float AnimationClip::getLength() const
{
	return length;
}

bool AnimationClip::isLooping() const
{
	return loop;
}

void AnimationClip::setLoop(bool shouldLoop)
{
	loop = shouldLoop;
}

void AnimationClip::addTrack(const AnimationTrack& track)
{
	tracks.push_back(track);
	updateLength();
}

const std::vector<AnimationTrack>& AnimationClip::getTracks() const
{
	return tracks;
}

std::vector<AnimationTrack>& AnimationClip::getTracks()
{
	return tracks;
}

void AnimationClip::updateLength()
{
	length = 0.0f;
	if ( tracks.empty() )
	{
		return;
	}

	// Calculate total duration for each property (sequential tracks sum up)
	// Then take the max across all properties (simultaneous properties)
	std::map<PropertyType, float> propertyTotalDuration;

	for ( const auto& track : tracks )
	{
		PropertyType prop = track.getProperty();
		if (!propertyTotalDuration.contains(prop) )
		{
			propertyTotalDuration[prop] = 0.0f;
		}
		propertyTotalDuration[prop] += track.getDuration();
	}

	// Clip length is the maximum total duration across all properties
	for ( const auto& [prop, totalDuration] : propertyTotalDuration )
	{
		length = std::max(length, totalDuration);
	}
}
