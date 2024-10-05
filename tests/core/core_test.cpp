//
// Created by pablo on 6/1/24.
//

#include <gtest/gtest.h>

#include "Core/Application.h"
#include "Core/Node.h"
#include "Core/NodePtr.h"
#include "Core/Timer.h"

using namespace IRC;

class TestNode final : public Node {
public:
    explicit TestNode(Node* parent = nullptr) : Node(parent) {}

protected:
    void update() override {}
};

class CoreFixture : public ::testing::Test {
public:
    void SetUp() override {
        // Code here will be called immediately after the constructor (right before each test).
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right before the destructor).
    }

    Application* app = nullptr;
};

TEST_F(CoreFixture, invalidAppInstance) {
    EXPECT_TRUE(Application::instance() == nullptr);
    EXPECT_TRUE(CoreFixture::app == nullptr);
}

TEST_F(CoreFixture, avoidWithoutApplicationInstance) {
    try {
        TestNode node;
        node.deleteLater();
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& err) {
        EXPECT_TRUE(true);
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST_F(CoreFixture, createApplicationInstance) {
    app = new Application();
    EXPECT_TRUE(Application::instance() != nullptr);
    EXPECT_TRUE(CoreFixture::app != nullptr);
    EXPECT_TRUE(Application::instance() == CoreFixture::app);
    // roots node are empty
    EXPECT_TRUE(Application::instance()->getRootNodesCount() == 0);
}

TEST_F(CoreFixture, createNodeRoot) {
    EXPECT_TRUE(Application::instance() != nullptr);
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    {
        TestNode node;
        EXPECT_TRUE(node.parent() == nullptr);
        EXPECT_TRUE(node.childrenCount() == 0);
        EXPECT_TRUE(node.isRoot());

        // App instance has one root node
        EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    }
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
}

TEST_F(CoreFixture, addChildConstructor) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    TestNode parent;
    TestNode child(&parent);
    EXPECT_TRUE(parent.childrenCount() == 1);
    EXPECT_TRUE(child.parent() == &parent);
    EXPECT_FALSE(child.isRoot());
    // the childs of child should be 0
    EXPECT_TRUE(child.childrenCount() == 0);
    // app instance has one root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
}

TEST_F(CoreFixture, addChildMethod) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    TestNode parent;
    TestNode child;
    parent.addChild(&child);
    EXPECT_TRUE(parent.childrenCount() == 1);
    EXPECT_TRUE(child.parent() == &parent);
    EXPECT_FALSE(child.isRoot());
    // the childs of child should be 0
    EXPECT_TRUE(child.childrenCount() == 0);
    // app instance has one root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
}

TEST_F(CoreFixture, setParent) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    TestNode parent;
    TestNode child;
    // The child at this moment should be root
    EXPECT_TRUE(child.isRoot());
    // The app at this moment should have 2 root nodes
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 2);
    // At this moment isChild should return false
    EXPECT_FALSE(parent.isChild(&child));
    // Change the parent
    child.setParent(&parent);
    // At this moment isChild should return true
    EXPECT_TRUE(parent.isChild(&child));
    // The child at this moment should not be root
    EXPECT_FALSE(child.isRoot());
    // The app at this moment should have 1 root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    // The parent should have 1 child
    EXPECT_TRUE(parent.childrenCount() == 1);
    // The child should have the parent as parent
    EXPECT_EQ(child.parent(), &parent);
    // The child should not be root
    EXPECT_FALSE(child.isRoot());
}

// Is child
TEST_F(CoreFixture, isChild) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    TestNode parent;
    TestNode child;
    EXPECT_FALSE(parent.isChild(&child));
    parent.addChild(&child);
    EXPECT_TRUE(parent.isChild(&child));
}

// Get child test
TEST_F(CoreFixture, getChild) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    TestNode parent;
    TestNode child;
    EXPECT_FALSE(parent.getChild(0));
    parent.addChild(&child);
    // The child at this moment should not be root
    EXPECT_FALSE(child.isRoot());
    // The app at this moment should have 1 root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    // The parent should have 1 child
    EXPECT_TRUE(parent.childrenCount() == 1);
    // The child should have the parent as parent
    EXPECT_EQ(child.parent(), &parent);
    // The child should not be root
    EXPECT_FALSE(child.isRoot());
    // The parent should have 1 child
    EXPECT_TRUE(parent.childrenCount() == 1);
    // The child should be the same memory address
    EXPECT_EQ(parent.getChild(0), &child);
}

// TESTS TO DELETE LATER
// TEST of deletion and delete later of Node pointers
TEST_F(CoreFixture, nodePtrDeleteLater) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 0);
    TestNode* parent = new TestNode();
    TestNode* child = new TestNode(parent);
    // The parent should be nullptr
    EXPECT_EQ(parent->parent(), nullptr);
    // The app should have 1 root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    // The parent should have 1 child
    EXPECT_EQ(parent->childrenCount(), 1);
    // The child should have the parent as parent
    EXPECT_EQ(child->parent(), parent);
    // The child should not be root
    EXPECT_FALSE(child->isRoot());
    // The application should have 2 alive nodes and non marked to delete
    EXPECT_EQ(Application::instance()->getAliveNodesCount(), 2);
    EXPECT_EQ(Application::instance()->getAliveNodesToDeleteCount(), 0);

    // The deleteLater of the child should not delete the child
    child->deleteLater();
    // The application should not be aware of the child to delete
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 0);
    EXPECT_EQ(parent->childrenToDeleteCount(), 1);
    EXPECT_TRUE(parent->getChild(0));
    {
        NodePtr nodePtr(child);
        EXPECT_TRUE(nodePtr);
    }
    // The parent::run function should delete the child
    parent->run();
    EXPECT_EQ(parent->childrenToDeleteCount(), 0);
    EXPECT_EQ(parent->childrenCount(), 0);
    // The app should have 1 root node
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    // The child should be deleted
    {
        NodePtr nodePtr(child);
        EXPECT_FALSE(nodePtr);
    }

    // From the parent, if the App::run is called and the parent is marked to delete later
    // the parent should be deleted
    EXPECT_FALSE(parent->parent());
    EXPECT_TRUE(parent->isRoot());
    parent->deleteLater();
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 1);
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
    {
        NodePtr nodePtr(parent);
        EXPECT_TRUE(nodePtr);
    }
    Application::instance()->run();
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 0);
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    {
        NodePtr nodePtr(parent);
        EXPECT_FALSE(nodePtr);
    }
}

// TEST NodePtr
TEST_F(CoreFixture, NodePtrFunctionality) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 0);
    TestNode* parent = new TestNode();
    TestNode* child = new TestNode(parent);
    // The NodePtr should be valid
    {
        NodePtr nodePtr(child);
        EXPECT_TRUE(nodePtr);
        EXPECT_TRUE(nodePtr.get() == child);
        EXPECT_TRUE(nodePtr.isAlive());
        EXPECT_FALSE(nodePtr.isMarkedToDelete());
    }
    // The deleteLater of the child should not delete the child
    child->deleteLater();
    // The application should not be aware of the child to delete
    {
        NodePtr nodePtr(child);
        EXPECT_TRUE(nodePtr);
        EXPECT_TRUE(nodePtr.get() == child);
        EXPECT_TRUE(nodePtr.isAlive());
        EXPECT_TRUE(nodePtr.isMarkedToDelete());
    }
    // The parent::run function should delete the child
    parent->run();
    // The child should be deleted
    {
        NodePtr nodePtr(child);
        EXPECT_FALSE(nodePtr);
        EXPECT_FALSE(nodePtr.isAlive());
        EXPECT_TRUE(nodePtr.isMarkedToDelete());
    }

    // From the parent, if the App::run is called and the parent is marked to delete later
    // the parent should be deleted
    EXPECT_FALSE(parent->parent());
    EXPECT_TRUE(parent->isRoot());
    // The NodePtr should be valid
    {
        NodePtr nodePtr(parent);
        EXPECT_TRUE(nodePtr);
        EXPECT_TRUE(nodePtr.get() == parent);
        EXPECT_TRUE(nodePtr.isAlive());
        EXPECT_FALSE(nodePtr.isMarkedToDelete());
    }
    parent->deleteLater();
    // test the parent pointer
    {
        NodePtr nodePtr(parent);
        EXPECT_TRUE(nodePtr);
        EXPECT_TRUE(nodePtr.get() == parent);
        EXPECT_TRUE(nodePtr.isAlive());
        EXPECT_TRUE(nodePtr.isMarkedToDelete());
    }
    Application::instance()->run();
    // The parent should be deleted
    {
        NodePtr nodePtr(parent);
        EXPECT_FALSE(nodePtr);
        EXPECT_FALSE(nodePtr.isAlive());
        EXPECT_TRUE(nodePtr.isMarkedToDelete());
    }
    // the app should have 0 root nodes
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    // and 0 nodes to delete
    EXPECT_EQ(Application::instance()->getRootNodesToDeleteCount(), 0);
    EXPECT_EQ(Application::instance()->getAliveNodesCount(), 0);
}

// TEST Timer
TEST_F(CoreFixture, TimerBasic) {
    Timer timer;
    ASSERT_TRUE(timer.isSingleShot());
    ASSERT_EQ(timer.remainingTime(), 0);
    ASSERT_EQ(timer.getInterval(), 0);
    ASSERT_FALSE(timer.isRunning());
}

TEST_F(CoreFixture, TimerSingleShot) {

    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    Timer timer;
    timer.setSingleShot(true);
    timer.setInterval(1000);
    ASSERT_EQ(timer.getInterval(), 1000);
    ASSERT_TRUE(timer.isSingleShot());
    bool called = false;
    timer.timeout.connect([&called]() { called = true; });
    timer.start();
    while (!called) {
        Application::instance()->loopOnce();
    }
    ASSERT_TRUE(called);
    // launch the timer again
    called = false;
    timer.start();
    while (!called) {
        Application::instance()->loopOnce();
    }
    ASSERT_TRUE(called);
    // Timer to control in 2 seconds that the main timer does not call the function
    // more than once
    Timer controlTimer;
    controlTimer.setInterval(2000);
    controlTimer.setSingleShot(true);
    bool calledControl = false;
    controlTimer.timeout.connect([&calledControl]() { calledControl = true; });

    int calledCount = 0;
    timer.timeout.connect([&calledCount]() { calledCount++; });
    timer.start();
    controlTimer.start();

    while (!calledControl) {
        Application::instance()->loopOnce();
    }
    ASSERT_EQ(calledCount, 1);
}

TEST_F(CoreFixture, TimerMultipleShot) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    Timer timer;
    timer.setSingleShot(false);
    timer.setInterval(1000);
    int count = 0;
    timer.timeout.connect([&count]() { count++; });

    bool controlCalled = false;
    Timer controlTimer;
    controlTimer.setInterval(2500);
    controlTimer.setSingleShot(true);
    controlTimer.timeout.connect([&controlCalled]() { controlCalled = true; });

    timer.start();
    controlTimer.start();
    while (!controlCalled) {
        Application::instance()->loopOnce();
    }
    ASSERT_EQ(count, 2);
}

TEST_F(CoreFixture, TimerSingleShotFunction) {
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
    bool called = false;
    Timer::singleShot(1000, [&called]() { called = true; });
    while (!called) {
        ASSERT_EQ(Application::instance()->getAliveNodesCount(), 1);
        EXPECT_EQ(Application::instance()->getRootNodesCount(), 1);
        Application::instance()->loopOnce();
    }
    // This 'loopOnce' call will delete the timer
    Application::instance()->loopOnce();
    ASSERT_TRUE(called);
    // check that the timer is deleted
    ASSERT_EQ(Application::instance()->getAliveNodesCount(), 0);
    EXPECT_EQ(Application::instance()->getRootNodesCount(), 0);
}
