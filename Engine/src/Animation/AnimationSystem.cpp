#include "Animation/AnimationSystem.h"

#include "Animation/Animator.h"

#include <algorithm>

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
	// Clear all animator references to this system before destroying
	// This prevents Animators from trying to unregister from a destroyed system
	for ( Animator* animator : animators )
	{
		if ( animator != nullptr )
		{
			animator->clearAnimationSystem();
		}
	}
	animators.clear();
}

void AnimationSystem::registerAnimator(Animator* animator)
{
	if ( animator == nullptr )
	{
		return;
	}

	// Check if already registered
	auto it = std::find(animators.begin(), animators.end(), animator);
	if ( it == animators.end() )
	{
		animators.push_back(animator);
	}
}

void AnimationSystem::unregisterAnimator(Animator* animator)
{
	if ( animator == nullptr )
	{
		return;
	}

	auto it = std::find(animators.begin(), animators.end(), animator);
	if ( it != animators.end() )
	{
		animators.erase(it);
	}
}

void AnimationSystem::update(float deltaTime)
{
	// Clean up destroyed animators first
	cleanupDestroyedAnimators();

	// Update all registered animators
	for ( Animator* animator : animators )
	{
		if ( animator != nullptr )
		{
			animator->update(deltaTime);
		}
	}

	// Clean up again after update (in case any were destroyed during update)
	cleanupDestroyedAnimators();
}

size_t AnimationSystem::getAnimatorCount() const
{
	return animators.size();
}

void AnimationSystem::cleanupDestroyedAnimators()
{
	animators.erase(std::remove_if(animators.begin(), animators.end(),
								   [](Animator* animator)
								   {
									   // Remove null pointers
									   // Note: We can't easily check if
									   // GameObject is destroyed without
									   // storing additional state, so we rely
									   // on Animator's onDestroy() to call
									   // unregisterAnimator()
									   return animator == nullptr;
								   }),
					animators.end());
}
