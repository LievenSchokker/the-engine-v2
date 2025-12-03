#include "Animation/AnimationSystem.h"

#include "Animation/Animator.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
	// No cleanup needed - we don't store references to animators
}

void AnimationSystem::update(Scene* scene, float deltaTime)
{
	if ( scene == nullptr )
	{
		return;
	}

	// Iterate through all GameObjects in the scene (pull-model, like Behaviour)
	for ( const auto& gameObject : scene->getGameObjects() )
	{
		// Only update animators on active GameObjects
		if ( gameObject == nullptr || !gameObject->getIsActive() )
		{
			continue;
		}

		// Get Animator component from this GameObject
		Animator* animator = gameObject->getComponent<Animator>();
		if ( animator != nullptr )
		{
			// Update the animator (it will check internally if it's playing)
			animator->update(deltaTime);
		}
	}
}
