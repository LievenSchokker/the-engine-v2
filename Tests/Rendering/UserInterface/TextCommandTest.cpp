#include <gtest/gtest.h>
#include "Rendering/UIRenderCommand.h"

#include <unordered_map>
#include <vector>
#include <cstdint>

class UICommandProcessor {
public:
    void reset() {
        commandQueue.clear();
        panelIndices.clear();
        panelElementIndices.clear();
        rootPanels.clear();
    }

    void submit(UIRenderCommand command) {
        size_t index = commandQueue.size();
        commandQueue.push_back(std::move(command));

        const auto& cmd = commandQueue.back();

        if (cmd.type == UICommandType::Panel) {
            panelIndices[cmd.panelId] = index;
            if (cmd.parentId == NO_PARENT) {
                rootPanels.push_back(cmd.panelId);
            } else {
                panelElementIndices[cmd.parentId].push_back(index);
            }
        } else {
            panelElementIndices[cmd.panelId].push_back(index);
        }
    }

    std::vector<UIRenderCommand> commandQueue;
    std::unordered_map<uint32_t, size_t> panelIndices;
    std::unordered_map<uint32_t, std::vector<size_t>> panelElementIndices;
    std::vector<uint32_t> rootPanels;
};

class UIRenderCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        processor.reset();
    }

    UICommandProcessor processor;
};


TEST_F(UIRenderCommandTest, RootPanelRegistersCorrectlyAndNestedPanelBecomesChildElement) {
    // Create root panel
    UIRenderCommand root;
    root.type = UICommandType::Panel;
    root.panelId = 1;
    root.parentId = NO_PARENT;
    root.x = 0; root.y = 0;
    root.width = 400; root.height = 300;
    processor.submit(root);

    // Create nested panel
    UIRenderCommand nested;
    nested.type = UICommandType::Panel;
    nested.panelId = 2;
    nested.parentId = 1;  // Child of root
    nested.x = 10; nested.y = 10;
    nested.width = 200; nested.height = 150;
    processor.submit(nested);

    // Root panel should be in rootPanels
    ASSERT_EQ(processor.rootPanels.size(), 1);
    EXPECT_EQ(processor.rootPanels[0], 1);

    // Both panels should be indexed
    EXPECT_TRUE(processor.panelIndices.count(1) > 0);
    EXPECT_TRUE(processor.panelIndices.count(2) > 0);

    // Nested panel should be an element of root panel
    ASSERT_TRUE(processor.panelElementIndices.count(1) > 0);
    EXPECT_EQ(processor.panelElementIndices[1].size(), 1);

    // Verify the nested panel's data is accessible via the index
    size_t nestedIndex = processor.panelElementIndices[1][0];
    EXPECT_EQ(processor.commandQueue[nestedIndex].panelId, 2);
    EXPECT_EQ(processor.commandQueue[nestedIndex].parentId, 1);
}

TEST_F(UIRenderCommandTest, MultiplePanelHierarchyMaintainsCorrectParentChildRelationships) {
    UIRenderCommand root;
    root.type = UICommandType::Panel;
    root.panelId = 1;
    root.parentId = NO_PARENT;
    processor.submit(root);

    UIRenderCommand childA;
    childA.type = UICommandType::Panel;
    childA.panelId = 2;
    childA.parentId = 1;
    processor.submit(childA);

    UIRenderCommand childB;
    childB.type = UICommandType::Panel;
    childB.panelId = 3;
    childB.parentId = 1;
    processor.submit(childB);

    UIRenderCommand grandchild;
    grandchild.type = UICommandType::Panel;
    grandchild.panelId = 4;
    grandchild.parentId = 2;
    processor.submit(grandchild);

    // Only one root
    ASSERT_EQ(processor.rootPanels.size(), 1);
    EXPECT_EQ(processor.rootPanels[0], 1);

    // Root has 2 children (childA and childB)
    ASSERT_EQ(processor.panelElementIndices[1].size(), 2);

    // childA has 1 child (grandchild)
    ASSERT_EQ(processor.panelElementIndices[2].size(), 1);
    size_t grandchildIndex = processor.panelElementIndices[2][0];
    EXPECT_EQ(processor.commandQueue[grandchildIndex].panelId, 4);

    // childB has no children
    EXPECT_EQ(processor.panelElementIndices[3].size(), 0);

    // All 4 panels are indexed
    EXPECT_EQ(processor.panelIndices.size(), 4);
}

TEST_F(UIRenderCommandTest, TextElementsGroupCorrectlyUnderTheirAssignedPanels) {
    // Create two panels
    UIRenderCommand panel1;
    panel1.type = UICommandType::Panel;
    panel1.panelId = 1;
    panel1.parentId = NO_PARENT;
    processor.submit(panel1);

    UIRenderCommand panel2;
    panel2.type = UICommandType::Panel;
    panel2.panelId = 2;
    panel2.parentId = NO_PARENT;
    processor.submit(panel2);

    // Add texts to different panels in mixed order
    UIRenderCommand text1;
    text1.type = UICommandType::Text;
    text1.panelId = 1;
    text1.text = "Panel 1 - Text A";
    processor.submit(text1);

    UIRenderCommand text2;
    text2.type = UICommandType::Text;
    text2.panelId = 2;
    text2.text = "Panel 2 - Text A";
    processor.submit(text2);

    UIRenderCommand text3;
    text3.type = UICommandType::Text;
    text3.panelId = 1;
    text3.text = "Panel 1 - Text B";
    processor.submit(text3);

    // Panel 1 should have 2 text elements
    ASSERT_EQ(processor.panelElementIndices[1].size(), 2);

    // Panel 2 should have 1 text element
    ASSERT_EQ(processor.panelElementIndices[2].size(), 1);

    // Verify correct texts are under correct panels
    size_t p1Text1Idx = processor.panelElementIndices[1][0];
    size_t p1Text2Idx = processor.panelElementIndices[1][1];
    size_t p2Text1Idx = processor.panelElementIndices[2][0];

    EXPECT_EQ(processor.commandQueue[p1Text1Idx].text, "Panel 1 - Text A");
    EXPECT_EQ(processor.commandQueue[p1Text2Idx].text, "Panel 1 - Text B");
    EXPECT_EQ(processor.commandQueue[p2Text1Idx].text, "Panel 2 - Text A");
}

TEST_F(UIRenderCommandTest, TextAndPanelElementsCanCoexistUnderSameParent) {
    // Root panel
    UIRenderCommand root;
    root.type = UICommandType::Panel;
    root.panelId = 1;
    root.parentId = NO_PARENT;
    processor.submit(root);

    // Text before nested panel
    UIRenderCommand text1;
    text1.type = UICommandType::Text;
    text1.panelId = 1;
    text1.text = "Header";
    processor.submit(text1);

    // Nested panel
    UIRenderCommand nested;
    nested.type = UICommandType::Panel;
    nested.panelId = 2;
    nested.parentId = 1;
    processor.submit(nested);

    // Text after nested panel
    UIRenderCommand text2;
    text2.type = UICommandType::Text;
    text2.panelId = 1;
    text2.text = "Footer";
    processor.submit(text2);

    // Text inside nested panel
    UIRenderCommand nestedText;
    nestedText.type = UICommandType::Text;
    nestedText.panelId = 2;
    nestedText.text = "Nested Content";
    processor.submit(nestedText);

    // Root has 3 elements: text, panel, text
    ASSERT_EQ(processor.panelElementIndices[1].size(), 3);

    // Nested panel has 1 element: text
    ASSERT_EQ(processor.panelElementIndices[2].size(), 1);

    // Verify order is preserved
    auto& rootElements = processor.panelElementIndices[1];
    EXPECT_EQ(processor.commandQueue[rootElements[0]].type, UICommandType::Text);
    EXPECT_EQ(processor.commandQueue[rootElements[0]].text, "Header");

    EXPECT_EQ(processor.commandQueue[rootElements[1]].type, UICommandType::Panel);
    EXPECT_EQ(processor.commandQueue[rootElements[1]].panelId, 2);

    EXPECT_EQ(processor.commandQueue[rootElements[2]].type, UICommandType::Text);
    EXPECT_EQ(processor.commandQueue[rootElements[2]].text, "Footer");

    // Verify nested content
    auto& nestedElements = processor.panelElementIndices[2];
    EXPECT_EQ(processor.commandQueue[nestedElements[0]].text, "Nested Content");
}