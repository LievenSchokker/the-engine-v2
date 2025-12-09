//
// Created by samle on 18/11/2025.
//

#include "TestBehaviours.h"


namespace engine_tests
{
    TestBehaviourBase::~TestBehaviourBase()
    {

    }


    void TestBehaviourBase::onAwake()
    {
        Behaviour::awake();
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


    void TestBehaviourBase::update(float deltaTime, GameWorld* gameWorld)
    {
		(void)deltaTime;  // Not used in tests
		Behaviour::update(deltaTime, gameWorld);
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