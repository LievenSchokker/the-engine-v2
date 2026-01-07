#include "Events/EventDispatcher/EventDispatcher.h"
#include "Events/Subscriptions/SubscriptionScope.h"
#include "Events/Subscriptions/SubscriptionHandle.h"
#include "Events/Event.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>


struct TestEvent : public Event<TestEvent>
{
    int value = 0;
    explicit TestEvent(int v = 0) : value(v) {}
};

struct AnotherEvent : public Event<AnotherEvent>
{
    std::string message;
    explicit AnotherEvent(std::string msg = "") : message(std::move(msg)) {}
};

class EventDispatcherTest : public ::testing::Test
{
protected:
    EventDispatcher dispatcher;
};

TEST_F(EventDispatcherTest, SubscribeAndDispatch)
{
    int receivedValue = 0;
    auto handle = dispatcher.subscribe<TestEvent>([&](const TestEvent& e) {
        receivedValue = e.value;
    });

    TestEvent event(42);
    dispatcher.dispatch(event);

    EXPECT_EQ(receivedValue, 42);
}

TEST_F(EventDispatcherTest, UnsubscribeStopsCallbacks)
{
    int callCount = 0;
    auto handle = dispatcher.subscribe<TestEvent>([&](const TestEvent&) {
        callCount++;
    });

    TestEvent event;
    dispatcher.dispatch(event);
    EXPECT_EQ(callCount, 1);

    dispatcher.unsubscribe(handle);
    dispatcher.dispatch(event);
    EXPECT_EQ(callCount, 1);
}

TEST_F(EventDispatcherTest, DoubleUnsubscribeIsSafe)
{
    auto handle = dispatcher.subscribe<TestEvent>([](const TestEvent&) {});

    dispatcher.unsubscribe(handle);
    EXPECT_FALSE(handle.isValid());

    dispatcher.unsubscribe(handle);
    EXPECT_FALSE(handle.isValid());
}

TEST_F(EventDispatcherTest, InvalidHandleUnsubscribeIsSafe)
{
    SubscriptionHandle invalidHandle(0, 0);
    invalidHandle.invalidate();
    dispatcher.unsubscribe(invalidHandle);
}

TEST_F(EventDispatcherTest, MultipleSubscribersReceiveEvents)
{
    int count1 = 0, count2 = 0, count3 = 0;

    auto h1 = dispatcher.subscribe<TestEvent>([&](const TestEvent&) { count1++; });
    auto h2 = dispatcher.subscribe<TestEvent>([&](const TestEvent&) { count2++; });
    auto h3 = dispatcher.subscribe<TestEvent>([&](const TestEvent&) { count3++; });

    TestEvent event;
    dispatcher.dispatch(event);

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
    EXPECT_EQ(count3, 1);
}

TEST_F(EventDispatcherTest, UnsubscribeOneDoesNotAffectOthers)
{
    int count1 = 0, count2 = 0;

    auto h1 = dispatcher.subscribe<TestEvent>([&](const TestEvent&) { count1++; });
    auto h2 = dispatcher.subscribe<TestEvent>([&](const TestEvent&) { count2++; });

    dispatcher.unsubscribe(h1);

    TestEvent event;
    dispatcher.dispatch(event);

    EXPECT_EQ(count1, 0);
    EXPECT_EQ(count2, 1);
}

TEST_F(EventDispatcherTest, UnsubscribeAllForType)
{
    int testCount = 0, anotherCount = 0;

    dispatcher.subscribe<TestEvent>([&](const TestEvent&) { testCount++; });
    dispatcher.subscribe<TestEvent>([&](const TestEvent&) { testCount++; });
    dispatcher.subscribe<AnotherEvent>([&](const AnotherEvent&) { anotherCount++; });

    dispatcher.unsubscribeAll<TestEvent>();

    TestEvent te;
    AnotherEvent ae;
    dispatcher.dispatch(te);
    dispatcher.dispatch(ae);

    EXPECT_EQ(testCount, 0);
    EXPECT_EQ(anotherCount, 1);
}


TEST_F(EventDispatcherTest, HandledEventStopsPropagation)
{
    int count = 0;

    dispatcher.subscribe<TestEvent>([&](const TestEvent& e) {
        count++;
        const_cast<TestEvent&>(e).handled = true;
    });
    dispatcher.subscribe<TestEvent>([&](const TestEvent&) {
        count++;
    });

    TestEvent event;
    dispatcher.dispatch(event);

    EXPECT_EQ(count, 1);
    EXPECT_TRUE(event.handled);
}

TEST_F(EventDispatcherTest, DispatchToNoSubscribersIsSafe)
{
    TestEvent event(123);
    dispatcher.dispatch(event);
}


class SubscriptionScopeTest : public ::testing::Test
{
protected:
    EventDispatcher dispatcher;
};

TEST_F(SubscriptionScopeTest, AutoUnsubscribesOnDestruction)
{
    int callCount = 0;

    {
        SubscriptionScope scope(dispatcher);
        scope.subscribe<TestEvent>([&](const TestEvent&) {
            callCount++;
        });

        TestEvent event;
        dispatcher.dispatch(event);
        EXPECT_EQ(callCount, 1);
    }

    TestEvent event;
    dispatcher.dispatch(event);
    EXPECT_EQ(callCount, 1);
}

TEST_F(SubscriptionScopeTest, MultipleSubscriptionsAllUnsubscribed)
{
    int count1 = 0, count2 = 0;

    {
        SubscriptionScope scope(dispatcher);
        scope.subscribe<TestEvent>([&](const TestEvent&) { count1++; });
        scope.subscribe<AnotherEvent>([&](const AnotherEvent&) { count2++; });

        TestEvent te;
        AnotherEvent ae;
        dispatcher.dispatch(te);
        dispatcher.dispatch(ae);

        EXPECT_EQ(count1, 1);
        EXPECT_EQ(count2, 1);
    }

    TestEvent te;
    AnotherEvent ae;
    dispatcher.dispatch(te);
    dispatcher.dispatch(ae);

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
}

TEST_F(SubscriptionScopeTest, EmptyScopeDestructionIsSafe)
{
    {
        SubscriptionScope scope(dispatcher);
        // No subscriptions added
    }
    SUCCEED();
}

TEST_F(SubscriptionScopeTest, SetDispatcherAfterConstruction)
{
    int callCount = 0;

    {
        SubscriptionScope scope;
        scope.setDispatcher(dispatcher);
        scope.subscribe<TestEvent>([&](const TestEvent&) {
            callCount++;
        });

        TestEvent event;
        dispatcher.dispatch(event);
        EXPECT_EQ(callCount, 1);
    }

    TestEvent event;
    dispatcher.dispatch(event);
    EXPECT_EQ(callCount, 1);
}

TEST_F(SubscriptionScopeTest, SubscribeWithoutDispatcherDoesNothing)
{
    int callCount = 0;

    SubscriptionScope scope;
    scope.subscribe<TestEvent>([&](const TestEvent&) {
        callCount++;
    });

    EXPECT_TRUE(scope.empty());
}

TEST_F(SubscriptionScopeTest, ManualUnsubscribeAll)
{
    int callCount = 0;

    SubscriptionScope scope(dispatcher);
    scope.subscribe<TestEvent>([&](const TestEvent&) { callCount++; });

    EXPECT_FALSE(scope.empty());
    EXPECT_EQ(scope.size(), 1);

    scope.unsubscribeAll();

    EXPECT_TRUE(scope.empty());
    EXPECT_EQ(scope.size(), 0);

    TestEvent event;
    dispatcher.dispatch(event);
    EXPECT_EQ(callCount, 0);
}

TEST_F(SubscriptionScopeTest, ScopeDestroyedBeforeDispatcherIsSafe)
{
    auto dispatcherPtr = std::make_unique<EventDispatcher>();
    int callCount = 0;

    {
        SubscriptionScope scope(*dispatcherPtr);
        scope.subscribe<TestEvent>([&](const TestEvent&) { callCount++; });
    }

    TestEvent event;
    dispatcherPtr->dispatch(event);
    EXPECT_EQ(callCount, 0);
}

TEST(SubscriptionHandleTest, NewHandleIsValid)
{
    SubscriptionHandle handle(1, 1);
    EXPECT_TRUE(handle.isValid());
}

TEST(SubscriptionHandleTest, InvalidateMarksInvalid)
{
    SubscriptionHandle handle(1, 1);
    handle.invalidate();
    EXPECT_FALSE(handle.isValid());
}

TEST(SubscriptionHandleTest, ZeroIdHandleCanBeInvalidated)
{
    SubscriptionHandle handle(0, 0);
    handle.invalidate();
    EXPECT_FALSE(handle.isValid());
}








TEST(EventSystemIntegration, MultipleScopesWithSameDispatcher)
{
    EventDispatcher dispatcher;
    int count1 = 0, count2 = 0;

    {
        SubscriptionScope scope1(dispatcher);
        scope1.subscribe<TestEvent>([&](const TestEvent&) { count1++; });

        {
            SubscriptionScope scope2(dispatcher);
            scope2.subscribe<TestEvent>([&](const TestEvent&) { count2++; });

            TestEvent event;
            dispatcher.dispatch(event);
            EXPECT_EQ(count1, 1);
            EXPECT_EQ(count2, 1);
        }  // scope2 destroyed

        TestEvent event;
        dispatcher.dispatch(event);
        EXPECT_EQ(count1, 2);
        EXPECT_EQ(count2, 1);  // scope2's callback unsubscribed
    }

    TestEvent event;
    dispatcher.dispatch(event);
    EXPECT_EQ(count1, 2);  // scope1's callback also unsubscribed
    EXPECT_EQ(count2, 1);
}

