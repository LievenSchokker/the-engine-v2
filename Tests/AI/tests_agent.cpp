//
// Created by samle on 03/12/2025.
//


#include "AI/Agent.h"
#include "Component/Transform.h"
#include "GameObject/GameObject.h"
#include "TestAgentModule.h"

#include <gtest/gtest.h>


class AgentTest : public ::testing::Test
{
    protected:
        GameObject* gameObject = nullptr;
        Agent* agent = nullptr;

        void SetUp() override
        {
            gameObject = new GameObject();
            agent = gameObject->addComponent<Agent>();

            agent->setMaxModuleForceMagnitude(100);
            agent->setMaxVelocityMagnitude(100);
        }

        void TearDown() override
        {
            delete gameObject;
            gameObject = nullptr;
            agent = nullptr;
        }
};

/// Tests to see if the agent behaviour is enabled under default circumstances
/// Meaning addComponent<agent> has been used on a GO, and Agent::onAwake has been called
TEST_F(AgentTest, AgentIsEnabledAfterAwake)
{
    agent->awake();
    ASSERT_TRUE(agent->getIsEnabled());
}

/// This tests to see if the Agent behaviour is disabled in the onAwake method if its inherited transform is not assigned.
/// Passes: Agent gets disabled correctly, Fails: Agent does not get disabled
TEST(AgentTests, AgentIsDisabledAfterAwakeIfTransformIsMissing)
{
    Agent* agent = new Agent();

    EXPECT_EQ(agent->getGameObject(), nullptr);
    EXPECT_EQ(agent->getTransform(), nullptr);

    agent->awake();

    ASSERT_FALSE(agent->getIsEnabled());
}

/// Tests if the computeModuleForce() method returns the vector computed by the assigned module.
TEST_F(AgentTest, ComputeModuleForce)
{
    agent->addAgentModule<TestAgentModule>(1);
    agent->computeModuleForce();

    ASSERT_EQ(agent->computeModuleForce(), Vector2::one());
}

/// Tests to see if the computeModuleForce returns a Vector2::zero() if the Agent does not have any modules
TEST_F(AgentTest, ComputeModuleForceReturnsVector2ZeroWhenNoModules)
{
    ASSERT_EQ(agent->computeModuleForce(), Vector2::zero());
}

/// Tests if the computeModuleForce method returns a computed vector based on weights.
TEST_F(AgentTest, WeightedComputeModuleForce)
{
    /// Set weight to 5.
    agent->addAgentModule<TestAgentModule>(5);

    ASSERT_EQ(agent->computeModuleForce(), Vector2(5.0f, 5.0f));
}

/// Tests if the ComputeModuleForce() method correctly truncates the computed vector to its maximum.
TEST_F(AgentTest, ComputeModuleForceTruncatesToMax)
{
    /// Set max magnitude to 10:
    agent->setMaxModuleForceMagnitude(10);

    /// Weight > max magnitude so computeModuleForce() should truncate.
    agent->addAgentModule<TestAgentModule>(15);

    Vector2 computed = agent->computeModuleForce();

    /// Computed magnitude should be truncated.
    EXPECT_TRUE(computed.magnitude() < 15);

    /// Computed magnitude has to be 10 (normalised() * maxMagnitude == 10)
    ASSERT_EQ(computed.magnitude(), 10);
}

/// Tests if the computeDesiredVelocity method correctly truncates return vector to maxVelocityMagnitude if exceeding.
TEST_F(AgentTest, ComputeDesiredVelocityTruncatesToMaxVelocity)
{
    agent->setMaxVelocityMagnitude(5);

    /// Weight > max magnitude so computeDesiredVelocity() should truncate.
    agent->addAgentModule<TestAgentModule>(10);

    Vector2 computed = agent->computeDesiredVelocity();

    /// Computed magnitude should be truncated.
    EXPECT_TRUE(computed.magnitude() < 10);

    /// Computed magnitude has to be 5 (normalised() * maxMagnitude == 5)
    ASSERT_EQ(computed.magnitude(), 5);
}

/// Tests if the get/setMaxVelocityMagnitude methods work as intended
TEST_F(AgentTest, GetSetMaxVelocityMagnitude)
{
    agent->setMaxVelocityMagnitude(5);
    float value = agent->getMaxVelocityMagnitude();
    ASSERT_EQ(value, 5);
}

/// Tests if the get/setMaxModuleForceMagnitude methods work as intended
TEST_F(AgentTest, GetSetMaxModuleForceMagnitude)
{
    agent->setMaxModuleForceMagnitude(10);
    float value = agent->getMaxModuleForceMagnitude();
    ASSERT_EQ(value, 10);
}

/// Tests if the update() method moves the agent's Transform correctly,
/// (correct == using computeDesiredVelocity())
TEST_F(AgentTest, UpdateMovesTransformAsExpected)
{
    agent->addAgentModule<TestAgentModule>(1);
    Vector2 initPos = agent->getTransform()->getPosition();

    agent->update(0.016f);

    Vector2 updatedPos = agent->getTransform()->getPosition();
    EXPECT_NE(initPos, updatedPos);

    ASSERT_EQ(updatedPos, initPos + Vector2::one());
}

/// Tests if the getModuleCount method works as intended,
/// Correctly returning the count of added modules to the agent.
TEST_F(AgentTest, GetModuleCount)
{
    ASSERT_EQ(agent->getModuleCount(), 0);
    agent->addAgentModule<TestAgentModule>(1);
    ASSERT_EQ(agent->getModuleCount(), 1);
}

/// Tests if the AddModule<T> method correctly adds a module
/// Also tests if the return value (bool) of the method works correctly:
/// True if added, false if not added.
TEST_F(AgentTest, AddModuleMethod)
{
    ASSERT_EQ(agent->getModuleCount(), 0);
    bool added = agent->addAgentModule<TestAgentModule>(1);

    ASSERT_EQ(agent->getModuleCount(), 1);
    ASSERT_TRUE(added);
}

/// Tests if the AddModule<T> method prevents duplicates, without crashing the program or throwing errors.
/// Also tests if the return value (bool) of the method works as intended:
/// True if added, false if not added.
TEST_F(AgentTest, AddModuleMethodPreventsDuplicates)
{
    ASSERT_EQ(agent->getModuleCount(), 0);
    bool added_1 = agent->addAgentModule<TestAgentModule>(1);
    bool added_2 = agent->addAgentModule<TestAgentModule>(1);
    bool added_3 = agent->addAgentModule<TestAgentModule>(1);

    ASSERT_EQ(agent->getModuleCount(), 1);
    ASSERT_TRUE(added_1);
    ASSERT_FALSE(added_2);
    ASSERT_FALSE(added_3);
}

/// Tests if the removeModule<T> method works as intended
/// Also tests if the return value is correct (bool):
/// True if removed, false if not removed
TEST_F(AgentTest, RemoveModuleMethod)
{
    ASSERT_EQ(agent->getModuleCount(), 0);

    agent->addAgentModule<TestAgentModule>(1);
    ASSERT_EQ(agent->getModuleCount(), 1);

    bool removed = agent->removeAgentModule<TestAgentModule>();
    ASSERT_EQ(agent->getModuleCount(), 0);
    ASSERT_TRUE(removed);
}

/// Tests if the removeModule<T> method works as intended when removing a non-added module;
/// Should not crash, throw an error, and should return FALSE.
TEST_F(AgentTest, RemoveModuleMethodForNonAddedModule)
{
    ASSERT_EQ(agent->getModuleCount(), 0);

    bool removed = agent->removeAgentModule<TestAgentModule>();
    ASSERT_FALSE(removed);
    ASSERT_EQ(agent->getModuleCount(), 0);
}

/// Tests if the GetModuleWeight<T> methods work as intended;
/// Returns the correct float value:
/// Has the module? -> module's weight
/// Does not have the module? -> no crash, returns 0 as weight.
TEST_F(AgentTest, GetModuleWeight)
{
    /// Test non existing module:
    float nonExistingModuleReturnValue = agent->getModuleWeight<TestAgentModule>();
    ASSERT_EQ(nonExistingModuleReturnValue, 0);

    /// Test exisiting module:
    float desiredWeight = 5;
    agent->addAgentModule<TestAgentModule>(desiredWeight);
    float retrieved = agent->getModuleWeight<TestAgentModule>();

    ASSERT_EQ(desiredWeight, retrieved);
}


/// Tests if the SetModuleWeight<T> methods work as intended;
/// Correctly setting the weight value,
/// And also correctly returning true if set
TEST_F(AgentTest, SetModuleWeight)
{
    float desiredWeight = 5;
    agent->addAgentModule<TestAgentModule>(0);
    float initialWeight = agent->getModuleWeight<TestAgentModule>();

    EXPECT_EQ(initialWeight, 0);

    bool success = agent->setModuleWeight<TestAgentModule>(desiredWeight);
    ASSERT_EQ(desiredWeight, agent->getModuleWeight<TestAgentModule>());
    ASSERT_TRUE(success);
}

/// Tests if setting the weight for a module the agent doesnt have works as intended;
/// No errors, throws, and returning false.
TEST_F(AgentTest, SetModuleWeightOnMissingModule)
{
    float desiredWeight = 5;

    bool success = agent->setModuleWeight<TestAgentModule>(desiredWeight);
    ASSERT_EQ(0, agent->getModuleWeight<TestAgentModule>());
    ASSERT_FALSE(success);
}

/// Tests if the getModuleStatus<T> method works as intended:
/// Returning the status of the module T
/// Returning INACTIVE for non-existing modules.
TEST_F(AgentTest, GetModuleStatus)
{
    /// 1) Test if non-existing module returns inactive state
    ASSERT_EQ(agent->getModuleCount(), 0);
    ModuleStatus nonExistingModuleStatus = agent->getModuleStatus<TestAgentModule>();
    ASSERT_EQ(nonExistingModuleStatus, ModuleStatus::INACTIVE);

    /// 2) Test if existing module returns correct state:
    agent->addAgentModule<TestAgentModule>(1);

    /// 'Default' status should be active.
    ModuleStatus initStatus = agent->getModuleStatus<TestAgentModule>();
    ASSERT_EQ(initStatus, ModuleStatus::ACTIVE);

    bool setSucceed = agent->setModuleStatus<TestAgentModule>(ModuleStatus::INACTIVE);
    ModuleStatus retrieved = agent->getModuleStatus<TestAgentModule>();

    /// Prevents test failing if setter is not working as intended.
    if (setSucceed)
        ASSERT_EQ(retrieved, ModuleStatus::INACTIVE);
}

/// Tests if the SetModuleStaatus<T> works as intended;
/// -Correctly sets the status of the T module
/// -No errors on non-existing modules,
/// -returning true if set went through,
/// -returning false if non-existing module
TEST_F(AgentTest, SetModuleStatus)
{
    /// 1) Test non-existing module set:
    EXPECT_EQ(agent->getModuleCount(), 0);
    bool nonExistingModuleSetSucceed = agent->setModuleStatus<TestAgentModule>(ModuleStatus::INACTIVE);
    ASSERT_FALSE(nonExistingModuleSetSucceed);

    /// 2) set on existing module
    agent->addAgentModule<TestAgentModule>(1);
    bool setSuccess = agent->setModuleStatus<TestAgentModule>(ModuleStatus::INACTIVE);
    ModuleStatus retrieved = agent->getModuleStatus<TestAgentModule>();
    ASSERT_TRUE(setSuccess);

    /// EXPECT instead of ASSERT because we use the get method to retrieve the vlaue (which might not work as intended).
    EXPECT_EQ(retrieved, ModuleStatus::INACTIVE);
}

/// Tests the HasAgentModule<T> method:
/// - Should return true if the agent has the module, false otherwise
/// - Should work in runtime; after add/remove methods have been called.
/// -Should return the same value as tryGetAgentModule<T> method
TEST_F(AgentTest, HasModule)
{
    /// 1) Return false for non-exisiting modules
    ASSERT_EQ(agent->getModuleCount(), 0);
    bool nonExistingModuleCheck = agent->hasAgentModule<TestAgentModule>();
    ASSERT_FALSE(nonExistingModuleCheck);

    /// 2) Return true after module has been added
    agent->addAgentModule<TestAgentModule>(1);
    ASSERT_EQ(agent->getModuleCount(), 1);
    bool hasExisting = agent->hasAgentModule<TestAgentModule>();
    ASSERT_TRUE(hasExisting);

    /// 3) Return false when module has been removed (runtime sim)
    ASSERT_EQ(agent->getModuleCount(), 1);
    agent->removeAgentModule<TestAgentModule>();
    ASSERT_EQ(agent->getModuleCount(), 0);
    bool hasRemoved = agent->hasAgentModule<TestAgentModule>();
    ASSERT_FALSE(hasRemoved);
}

/* NOTE: These tests are commented out because the tryGetModule<T> methods are PRIVATE methods of Agent, so they cannot be called here.
 * However, we can make them temporary public and run these tests to see if they work as intended.
 *
/// Tests if the tryGetModule<T>(param_1) method works as intended:
/// - Return bool == true if agent has the module, false otherwise
/// - Assigns the module to the out parameter if the module exists
/// - Assigns 'nullptr' to the out parameter if the module does not exist
TEST_F(AgentTest, TryGetModuleSingleParam)
{
    /// 1) Non-existing module;
    /// Return false, out param == nullptr
    ASSERT_EQ(agent->getModuleCount(), 0);
    ModuleData* retrievedModule;
    bool tryGetNonExistingModule = agent->tryGetAgentModule<TestAgentModule>(retrievedModule);
    ASSERT_FALSE(tryGetNonExistingModule);
    ASSERT_EQ(retrievedModule, nullptr);

    /// 2) Tryget an exisiting module:
    agent->addAgentModule<TestAgentModule>(1);
    ASSERT_EQ(agent->getModuleCount(), 1);
    bool hasExisting = agent->tryGetAgentModule<TestAgentModule>(retrievedModule);
    ASSERT_TRUE(hasExisting);
    ASSERT_NE(retrievedModule, nullptr);
}

/// Tests if the tryGetModule<T>(param_1, param_2) method works as intended:
/// - Return bool == true if agent has the module, false otherwise
/// - Assigns the module to the out parameter if the module exists
/// - Assigns 'nullptr' to the out parameter if the module does not exist
/// - Assigns the correct index of the module in the stored vector<> to the out index param if the module exists
/// - Assigns -1 to the out index param if the module does not exist


TEST_F(AgentTest, TryGetModuleDoubleParam)
{
    /// 1) Non-existing module;
    /// Return false, out param == nullptr
    ASSERT_EQ(agent->getModuleCount(), 0);

    ModuleData* retrievedModule;
    size_t retrievedIndex;

    bool tryGetNonExistingModule = agent->tryGetAgentModule<TestAgentModule>(retrievedModule, retrievedIndex);
    ASSERT_FALSE(tryGetNonExistingModule);
    ASSERT_EQ(retrievedModule, nullptr);
    ASSERT_EQ(retrievedIndex, -1); /// Non existing modules should return an index of -1.

    /// 2) Tryget an exisiting module:
    agent->addAgentModule<TestAgentModule>(1);
    ASSERT_EQ(agent->getModuleCount(), 1);
    bool hasExisting = agent->tryGetAgentModule<TestAgentModule>(retrievedModule, retrievedIndex);
    ASSERT_TRUE(hasExisting);
    ASSERT_NE(retrievedModule, nullptr);
    ASSERT_EQ(retrievedIndex, 0);
}
*
*  END OF PRIVATE TRYGETMODULE<T> TESTS
 */
