//
// Created by samle on 18/11/2025.
//

#include "TestBehaviour.h"


void engine_tests::TestBehaviour::awake()
{
    Behaviour::awake();
    awakeCalled = true;
}


void engine_tests::TestBehaviour::onEnable()
{
    Behaviour::onEnable();
    enableCalled = true;
}



void engine_tests::TestBehaviour::start()
{
    Behaviour::start();
    startCalled = true;
}


void engine_tests::TestBehaviour::update()
{
    Behaviour::update();
    updateCalled = true;
}


void engine_tests::TestBehaviour::onDisable()
{
    Behaviour::onDisable();
    disableCalled = true;
}


void engine_tests::TestBehaviour::onDestroy()
{
    Behaviour::onDestroy();
    destroyCalled = true;
}
