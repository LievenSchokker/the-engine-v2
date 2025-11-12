//
// Created by samle on 12/11/2025.
//



#include <gtest/gtest.h>
#include "../include/core/game_object/GameObject.h"



TEST(GameObjectTests, HasTransformAfterConstruction)
{
    GameObject go;
    EXPECT_NE(go.getTransform(), nullptr);
}

TEST(GameObjectTests, HasComponentManagerAfterConstruction)
{
    GameObject go;
    EXPECT_NE(go.getComponentManager(), nullptr);
}