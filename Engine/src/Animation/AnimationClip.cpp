#include "Animation/AnimationClip.h"

#include "Animation/AnimationCurve.h"

#include <algorithm>
#include <map>

AnimationClip::AnimationClip()
    : name("")
    , length(0.0f)
    , loop(false)
{
}

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

	for ( const auto& [prop, totalDuration] : propertyTotalDuration )
	{
		length = std::max(length, totalDuration);
	}
}
void AnimationClip::serialize(CerealWriteArchive& archive) const
{
    archive.process(name);

    uint8_t loopFlag = loop ? 1 : 0;
    archive.process(loopFlag);

    uint32_t trackCount = static_cast<uint32_t>(tracks.size());
    archive.process(trackCount);

    for (const auto& track : tracks)
    {
        track.serialize(archive);
    }
}

void AnimationClip::deserialize(ReadArchive& archive)
{
    archive.process(name);

    uint8_t loopFlag;
    archive.process(loopFlag);
    loop = (loopFlag != 0);

    uint32_t trackCount;
    archive.process(trackCount);

    tracks.clear();
    tracks.reserve(trackCount);

    for (uint32_t i = 0; i < trackCount; ++i)
    {
        AnimationTrack track;
        track.deserialize(archive);
        tracks.push_back(std::move(track));
    }

    updateLength();
}