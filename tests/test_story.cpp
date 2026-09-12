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

    // 4. Kiểm thử nạp data/story.json
    std::string storyPath = "data/story.json";
    if (!std::ifstream(storyPath).good()) storyPath = "../data/story.json";
    StoryGraph fileStory;
    bool loadedStory = fileStory.loadStoryGraph(storyPath);
    assert(loadedStory == true);
    assert(fileStory.getNodeCount() >= 6);
    
    StoryNode startNode = fileStory.getCurrentNode();
    assert(startNode.id == "node_01" || startNode.id == "village_start");

    // 5. Test DataLoader functions
    std::string itemsPath = "data/items.json";
    if (!std::ifstream(itemsPath).good()) itemsPath = "../data/items.json";
    auto items = DataLoader::loadItems(itemsPath);
    assert(!items.empty());

    std::string enemiesPath = "data/enemies.json";
    if (!std::ifstream(enemiesPath).good()) enemiesPath = "../data/enemies.json";
    auto enemies = DataLoader::loadEnemies(enemiesPath);
    assert(!enemies.empty());

    std::cout << "[PASS] All StoryGraph unit tests (including JSON loader) passed successfully!\n";
    return 0;
}
