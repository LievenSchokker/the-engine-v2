#include "Animation/AnimationClip.h"
#include "Animation/AnimationCurve.h"
#include "Animation/AnimationSystem.h"
#include "Animation/AnimationTrack.h"
#include "Animation/Animator.h"
#include "GameObject/GameObject.h"
#include "GameObject/Vector2.h"
#include "Scene/Scene.h"

#include <gtest/gtest.h>

class AnimationSystemTest: public ::testing::Test
{
   protected:
	void SetUp() override
	{
		system = std::make_unique<AnimationSystem>();
		scene = std::make_unique<Scene>("TestScene");
	}

	void TearDown() override
	{
	}

	std::unique_ptr<AnimationSystem> system;
	std::unique_ptr<Scene> scene;
};

TEST_F(AnimationSystemTest, UpdateFindsAndUpdatesAnimators)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	Animator* animator = go->addComponent<Animator>();

	AnimationClip clip("TestClip", false);
	AnimationCurve curve(EasingType::Linear);
	Vector2 fromPos{0.0f, 0.0f};
	Vector2 toPos{100.0f, 100.0f};

	AnimationTrack track(TargetType::Transform, PropertyType::Position, 1.0f,
						 false, fromPos, toPos, curve);
	clip.addTrack(track);

	animator->play(&clip);
	scene->addGameObject(std::move(go));

	// AnimationSystem should find and update the animator
	system->update(scene.get(), 0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 0.5f);

	system->update(scene.get(), 0.5f);
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), 1.0f);
}

TEST_F(AnimationSystemTest, UpdateMultipleAnimators)
{
	auto go1 = std::make_unique<GameObject>();
	go1->setName("Object1");
	Animator* animator1 = go1->addComponent<Animator>();

	auto go2 = std::make_unique<GameObject>();
	go2->setName("Object2");
	Animator* animator2 = go2->addComponent<Animator>();

	AnimationClip clip("TestClip", false);
	animator1->play(&clip);
	animator2->play(&clip);

	scene->addGameObject(std::move(go1));
	scene->addGameObject(std::move(go2));

	// AnimationSystem should find and update both animators
	system->update(scene.get(), 0.1f);
	EXPECT_FLOAT_EQ(animator1->getCurrentTime(), 0.1f);
	EXPECT_FLOAT_EQ(animator2->getCurrentTime(), 0.1f);
}

TEST_F(AnimationSystemTest, UpdateDoesNotUpdateInactiveGameObjects)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	go->setActive(false);  // Make GameObject inactive
	Animator* animator = go->addComponent<Animator>();

	AnimationClip clip("TestClip", false);
	animator->play(&clip);

	scene->addGameObject(std::move(go));

	float initialTime = animator->getCurrentTime();
	system->update(scene.get(), 0.5f);

	// Time should not advance for inactive GameObjects
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), initialTime);
}

TEST_F(AnimationSystemTest, UpdateDoesNotUpdateNonPlayingAnimators)
{
	auto go = std::make_unique<GameObject>();
	go->setName("TestObject");
	Animator* animator = go->addComponent<Animator>();

	AnimationClip clip("TestClip", false);
	// Don't call play() - animator is not playing

	scene->addGameObject(std::move(go));

	float initialTime = animator->getCurrentTime();
	system->update(scene.get(), 0.5f);

	// Time should not advance for non-playing animators
	EXPECT_FLOAT_EQ(animator->getCurrentTime(), initialTime);
}

TEST_F(AnimationSystemTest, UpdateHandlesNullScene)
{
	// Should not crash when scene is nullptr
	system->update(nullptr, 0.5f);
	EXPECT_TRUE(true);	// If we get here, no crash occurred
}
