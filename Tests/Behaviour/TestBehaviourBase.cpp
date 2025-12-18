#include "TestBehaviours.h"
#include "Core/GameWorld.h"

namespace engine_tests
{
    TestBehaviourBase::~TestBehaviourBase()
    {

    }


    void TestBehaviourBase::onAwake()
    {
        auto world = std::make_shared<GameWorld>();
        Behaviour::awake(*world);
        awakeCalled = true;
    }


    void TestBehaviourBase::onEnable()
    {
        Behaviour::onEnable();
        enableCalled = true;
    }


    void TestBehaviourBase::onStart()
    {
        Behaviour::start();
        startCalled = true;
    }


    void TestBehaviourBase::update(double deltaTime, const GameWorld& world)
    {
		(void)deltaTime;  // Not used in tests
		Behaviour::update(deltaTime, world);
		updateCalled = true;
    }


    void TestBehaviourBase::onDisable()
    {
        Behaviour::onDisable();
        disableCalled = true;
    }


    void TestBehaviourBase::onDestroy()
    {
        Behaviour::onDestroy();
        destroyCalled = true;
    }
}