#include "StoryGraph.h"
#include "DataLoader.h"
#include <cassert>
#include <iostream>
#include <fstream>

int main() {
    // 1. Kiểm thử đồ thị mặc định
    StoryGraph story;
    StoryNode node = story.getCurrentNode();
    assert(node.id == "start");
    assert(node.choices.size() == 2);

    story.addNode({"forest_path", "Deep inside the forest...", EventType::NORMAL, {}});
    assert(story.selectChoice(0) == true);
    assert(story.getCurrentNode().id == "forest_path");

    // 2. Test JSON parsing from string
    std::string testJson = R"({
        "startNodeId": "room1",
        "nodes": [
            {
                "id": "room1",
                "text": "Phòng số 1",
                "type": "NORMAL",
                "choices": [
                    { "text": "Đi sang phòng 2", "nextNodeId": "room2", "requiredFlag": "", "setFlag": "visited_room1" }
                ]
            },
            {
                "id": "room2",
                "text": "Phòng số 2 - Kho báu",
                "type": "ITEM",
                "choices": [
                    { "text": "Đến cửa thoát", "nextNodeId": "exit_room", "requiredFlag": "", "setFlag": "" }
                ]
            },
            {
                "id": "exit_room",
                "text": "Lối thoát",
                "type": "ENDING",
                "choices": []
            }
        ]
    })";

    StoryGraph jsonStory;
    assert(jsonStory.loadFromJsonString(testJson) == true);
    assert(jsonStory.getCurrentNode().id == "room1");
    assert(jsonStory.getCurrentNode().choices.size() == 1);
    
    // Test selecting choice
    assert(jsonStory.selectChoice(0) == true);
    assert(jsonStory.getCurrentNode().id == "room2");
    assert(jsonStory.getFlag("visited_room1") == true);
    assert(jsonStory.getCurrentNode().type == EventType::ITEM);

    // Test moving to ending
    assert(jsonStory.selectChoice(0) == true);
    assert(jsonStory.getCurrentNode().id == "exit_room");
    assert(jsonStory.isEnding() == true);

    // 3. Test invalid JSON handling
    StoryGraph brokenStory;
    assert(brokenStory.loadFromJsonString("{ invalid json content ...") == false);
    assert(brokenStory.loadFromJsonString("{\"nodes\": []}") == false);

    // 4. Kiểm thử nạp data/story.json & Toàn vẹn đồ thị (Dead-end & Link verification)
    std::string storyPath = "data/story.json";
    if (!std::ifstream(storyPath).good()) storyPath = "../data/story.json";
    StoryGraph fileStory;
    bool loadedStory = fileStory.loadStoryGraph(storyPath);
    assert(loadedStory == true);
    assert(fileStory.getNodeCount() >= 22);
    
    StoryNode startNode = fileStory.getCurrentNode();
    assert(startNode.id == "Node01");

    // Kiểm tra validateGraph trên file dữ liệu chính thức
    std::vector<std::string> validationErrors;
    bool isValid = fileStory.validateGraph(validationErrors);
    if (!isValid) {
        for (const auto& err : validationErrors) {
            std::cerr << "[StoryGraph Error] " << err << "\n";
        }
    }
    assert(isValid == true);
    assert(validationErrors.empty());

    // 5. Kiểm tra tính năng phát hiện Dead-end và Broken link
    StoryGraph brokenGraph;
    brokenGraph.addNode({"dead_node", "A lonely room with no exit", EventType::NORMAL, {}});
    brokenGraph.setCurrentNodeId("dead_node");
    std::vector<std::string> brokenErrors;
    assert(brokenGraph.validateGraph(brokenErrors) == false);
    assert(!brokenErrors.empty());

    StoryGraph danglingGraph;
    danglingGraph.addNode({"start_node", "Start", EventType::NORMAL, {{"Go to nowhere", "ghost_node"}}});
    danglingGraph.setCurrentNodeId("start_node");
    std::vector<std::string> danglingErrors;
    assert(danglingGraph.validateGraph(danglingErrors) == false);
    assert(!danglingErrors.empty());

    // 6. Kiểm tra canSelectChoice và requiredFlag
    StoryGraph flagGraph;
    StoryNode fNode("flag_room", "Locked room", EventType::NORMAL);
    fNode.choices.push_back({"Mở cửa (cần chìa khóa)", "next_room", "has_key", "unlocked"});
    flagGraph.addNode(fNode);
    flagGraph.addNode({"next_room", "Next room", EventType::ENDING, {}});
    flagGraph.setCurrentNodeId("flag_room");

    assert(flagGraph.canSelectChoice(0) == false);
    assert(flagGraph.selectChoice(0) == false);
    assert(flagGraph.getCurrentNodeId() == "flag_room");

    flagGraph.setFlag("has_key", true);
    assert(flagGraph.canSelectChoice(0) == true);
    assert(flagGraph.selectChoice(0) == true);
    assert(flagGraph.getCurrentNodeId() == "next_room");
    assert(flagGraph.getFlag("unlocked") == true);
    assert(flagGraph.isEnding() == true);

    // 7. Test DataLoader functions
    std::string itemsPath = "data/items.json";
    if (!std::ifstream(itemsPath).good()) itemsPath = "../data/items.json";
    auto items = DataLoader::loadItems(itemsPath);
    assert(!items.empty());

    std::string enemiesPath = "data/enemies.json";
    if (!std::ifstream(enemiesPath).good()) enemiesPath = "../data/enemies.json";
    auto enemies = DataLoader::loadEnemies(enemiesPath);
    assert(!enemies.empty());

    std::cout << "[PASS] All StoryGraph unit tests (validateGraph, Dead-ends, Choices & Flags) passed successfully!\n";
    return 0;
}
