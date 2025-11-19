//
// Created by samle on 19/11/2025.
//
#include "../Component/TestComponents.h"
#include <gtest/gtest.h>

namespace engine_tests
{
    TEST(TestComponentsTests, InstanceCountTest)
    {
        GameObject go;
        TestComponentOne* testComponentOne = go.addComponent<TestComponentOne>();
        TestComponentTwo* testComponentTwo= go.addComponent<TestComponentTwo>();
        TestComponentThree* testComponentThree = go.addComponent<TestComponentThree>();

        EXPECT_EQ(TestComponentBase::instanceCount, 3);

        /// This function internally destroys all Components attached to the GO, deleting them from memeory.
        go.onSceneDestroy();
        EXPECT_EQ(TestComponentBase::instanceCount, 0);

    }
}
