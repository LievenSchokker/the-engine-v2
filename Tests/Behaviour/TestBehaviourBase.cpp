//
// Created by samle on 18/11/2025.
//

#include "TestBehaviours.h"


namespace engine_tests
{
    TestBehaviourBase::~TestBehaviourBase()
    {

    }


    void TestBehaviourBase::awake()
    {
        Behaviour::awake();
        awakeCalled = true;
    }


    void TestBehaviourBase::onEnable()
    {
        Behaviour::onEnable();
        enableCalled = true;
    }


    void TestBehaviourBase::start()
    {
        Behaviour::start();
        startCalled = true;
    }


    void TestBehaviourBase::update()
    {
        Behaviour::update();
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