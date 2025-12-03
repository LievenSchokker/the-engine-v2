#include "Animation/AnimationSystem.h"
#include "Animation/Animator.h"
#include "Animation/AnimationClip.h"
#include "Animation/AnimationTrack.h"
#include "Animation/AnimationCurve.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"
#include <gtest/gtest.h>

class AnimationSystemTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		system = std::make_unique<AnimationSystem>();
	}
	
	void TearDown() override {}
	
	std::unique_ptr<AnimationSystem> system;
};

TEST_F(AnimationSystemTest, Constructor)
{
	EXPECT_EQ(system->getAnimatorCount(), 0);
}

TEST_F(AnimationSystemTest, Register)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	Animator* animator = go->addComponent<Animator>();
	animator->setAnimationSystem(system.get());
	
	AnimationClip clip("TestClip", false);
	animator->play(&clip);
	
	EXPECT_EQ(system->getAnimatorCount(), 1);
}

TEST_F(AnimationSystemTest, Unregister)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	Animator* animator = go->addComponent<Animator>();
	animator->setAnimationSystem(system.get());
	
	AnimationClip clip("TestClip", false);
	animator->play(&clip);
	EXPECT_EQ(system->getAnimatorCount(), 1);
	
	animator->stop();
	animator->setAnimationSystem(nullptr);
	
	// Note: Unregister is called automatically by Animator destructor
	// For testing, we manually unregister
        system->unregisterAnimator(animator);
	EXPECT_EQ(system->getAnimatorCount(), 0);
}

TEST_F(AnimationSystemTest, StepUpdatesAnimators)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	Animator* animator = go->addComponent<Animator>();
	animator->setAnimationSystem(system.get());
	
	AnimationClip clip("TestClip", false);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};
	
	AnimationTrack track(TargetType::Transform,
	                    PropertyType::Position,
	                    1.0f,
	                    false,
	                    fromPos,
	                    toPos,
	                    curve);
	clip.addTrack(track);
	
	animator->play(&clip);
	
        system->step(0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.5f);
	
        system->step(0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 1.0f);
}

TEST_F(AnimationSystemTest, MultipleAnimators)
{
	auto go1 = std::make_unique<GameObject>();
	go1->setName("Object1");
	Animator* animator1 = go1->addComponent<Animator>();
	animator1->setAnimationSystem(system.get());
	
	auto go2 = std::make_unique<GameObject>();
	go2->setName("Object2");
	Animator* animator2 = go2->addComponent<Animator>();
	animator2->setAnimationSystem(system.get());
	
	AnimationClip clip("TestClip", false);
	animator1->play(&clip);
	animator2->play(&clip);
	
	EXPECT_EQ(system->getAnimatorCount(), 2);
	
        system->step(0.1f);
	EXPECT_FLOAT_EQ(animator1->getCurrentTime(), 0.1f);
	EXPECT_FLOAT_EQ(animator2->getCurrentTime(), 0.1f);
}

TEST_F(AnimationSystemTest, StepDoesNotUpdateInactiveAnimators)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	go->setActive(false); // Make GameObject inactive
	Animator* animator = go->addComponent<Animator>();
	animator->setAnimationSystem(system.get());
	
	AnimationClip clip("TestClip", false);
	animator->play(&clip);
	
	float initialTime = animator->getCurrentTime();
        system->step(0.5f);
	
	// Time should not advance for inactive GameObjects
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), initialTime);
}

