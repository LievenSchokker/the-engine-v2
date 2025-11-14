// //
// // Created by samle on 14/11/2025.
// //
//
// TEST(GameObjectTests, AddTransformDirectlyIsNotAllowed)
// {
//     GameObject go;
//     go.addComponent<Transform>();
//     EXPECT_NE(go.getTransform(), nullptr);
//     EXPECT_EQ(go.getComponentManager()->getComponent<Transform>(), nullptr);
// }
//
// TEST(GameObjectTests, ComponentManagerRemovesAllComponents)
// {
//     GameObject go;
//     go.addComponent<TestComponentOne>();
//     go.addComponent<TestComponentTwo>();
//     go.addComponent<TestComponentThree>();
//
//     bool componentCountBiggerThanZero1 = go.getComponentManager()->components.size() > 0;
//     EXPECT_EQ(componentCountBiggerThanZero1, true);
//     go.getComponentManager()->removeAllComponents();
//     bool componentCountBiggerThanZero2 = go.getComponentManager()->components.size() > 0;
//
//     EXPECT_EQ(componentCountBiggerThanZero2, false);
// }
