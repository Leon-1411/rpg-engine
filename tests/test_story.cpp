#include "StoryGraph.h"
#include <cassert>
#include <iostream>

int main() {
    // 1. Kiểm thử đồ thị mặc định
    StoryGraph story;
    StoryNode node = story.getCurrentNode();
    assert(node.id == "start");
    assert(node.choices.size() == 2);

    story.addNode({"forest_path", "Deep inside the forest...", EventType::NORMAL, {}});
    assert(story.selectChoice(0) == true);
    assert(story.getCurrentNode().id == "forest_path");

    // 2. Kiểm thử nạp data/story.json
    StoryGraph jsonStory;
    bool loaded = jsonStory.loadStoryGraph("data/story.json");
    assert(loaded == true);
    assert(jsonStory.getNodeCount() == 12);
    
    StoryNode startNode = jsonStory.getCurrentNode();
    assert(startNode.id == "node_01");
    assert(startNode.choices.size() == 2);
    assert(startNode.choices[0].nextNodeId == "node_02");
    assert(startNode.choices[1].nextNodeId == "node_03");

    // Kiểm thử di chuyển nhánh lựa chọn
    assert(jsonStory.selectChoice(0) == true);
    assert(jsonStory.getCurrentNode().id == "node_02");
    assert(jsonStory.getCurrentNode().type == EventType::BATTLE);

    // Kiểm thử di chuyển trực tiếp tới các node kết thúc
    jsonStory.moveToNode("node_06");
    assert(jsonStory.isEnding() == true);

    jsonStory.moveToNode("node_08");
    assert(jsonStory.isEnding() == true);

    std::cout << "[PASS] All StoryGraph unit tests (including JSON loader) passed successfully!\n";
    return 0;
}

