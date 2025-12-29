//
// Created by samle on 19/11/2025.
//

#include "TestComponents.h"

namespace engine_tests
{
    int TestComponentBase::instanceCount = 0;
    int TestComponentBase::destroyCount = 0;

    TestComponentBase::TestComponentBase()
    {
        instanceCount++;
    }

    TestComponentBase::~TestComponentBase()
    {
        instanceCount--;
    }

    void TestComponentBase::onDestroy()
    {
        destroyCalled = true;
        destroyCount++;
    }
}
