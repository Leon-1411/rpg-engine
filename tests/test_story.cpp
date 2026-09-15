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

    // 2. Test JSON parsing from string with flags and types
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

    StoryGraph jsonStringStory;
    assert(jsonStringStory.loadFromJsonString(testJson) == true);
    assert(jsonStringStory.getCurrentNode().id == "room1");
    assert(jsonStringStory.getCurrentNode().choices.size() == 1);
    
    // Test selecting choice
    assert(jsonStringStory.selectChoice(0) == true);
    assert(jsonStringStory.getCurrentNode().id == "room2");
    assert(jsonStringStory.getFlag("visited_room1") == true);
    assert(jsonStringStory.getCurrentNode().type == EventType::ITEM);

    // Test moving to ending
    assert(jsonStringStory.selectChoice(0) == true);
    assert(jsonStringStory.getCurrentNode().id == "exit_room");
    assert(jsonStringStory.isEnding() == true);

    // 3. Test SaveManager getter/setter helper functions
    jsonStringStory.setCurrentNodeId("room1");
    assert(jsonStringStory.getCurrentNodeId() == "room1");
    std::unordered_map<std::string, bool> customFlags = {{"boss_defeated", true}, {"key_found", false}};
    jsonStringStory.setStoryFlags(customFlags);
    assert(jsonStringStory.getStoryFlags().size() == 2);
    assert(jsonStringStory.getFlag("boss_defeated") == true);
    jsonStringStory.clearStoryFlags();
    assert(jsonStringStory.getStoryFlags().empty());

    // 4. Test invalid JSON handling
    StoryGraph brokenStory;
    assert(brokenStory.loadFromJsonString("{ invalid json content ...") == false);
    assert(brokenStory.loadFromJsonString("{\"nodes\": []}") == false);

    // 5. Kiểm thử nạp data/story.json thực tế
    std::string storyPath = "data/story.json";
    if (!std::ifstream(storyPath).good()) storyPath = "../data/story.json";
    StoryGraph jsonStory;
    bool loaded = jsonStory.loadStoryGraph(storyPath);
    assert(loaded == true);
    assert(jsonStory.getNodeCount() >= 6);
    
    StoryNode startNode = jsonStory.getCurrentNode();
    assert(startNode.id == "node_01");
    assert(startNode.choices.size() >= 2);

    // Kiểm thử nạp NPC và Dialogues từ data/story.json
    assert(startNode.npcName == "Trưởng Làng Eldrin");
    assert(startNode.dialogues.size() >= 2);

    // 6. Kiểm thử cơ chế Hội thoại rẽ nhánh (Branching Dialogue Tree)
    jsonStory.moveToNode("node_01");
    assert(jsonStory.isInDialogue() == true);
    DialogueNode dNode1 = jsonStory.getCurrentDialogueNode();
    assert(dNode1.id == "d01_start");
    assert(dNode1.choices.size() == 4);

    // Đáp thoại [0]: Hỏi về Rừng Ma -> Rẽ sang nhánh thoại d01_forest_info
    assert(jsonStory.selectDialogueChoice(0) == true);
    assert(jsonStory.isInDialogue() == true);
    assert(jsonStory.getCurrentDialogueNode().id == "d01_forest_info");

    // Đáp thoại [0] của d01_forest_info: Quyết tâm vào rừng -> Dẫn tới Story Node node_02 (COMBAT)
    assert(jsonStory.selectDialogueChoice(0) == true);
    assert(jsonStory.getCurrentNode().id == "node_02");
    assert(jsonStory.getCurrentNode().type == EventType::BATTLE);

    // Tại node_02: Hội thoại với Quỷ Bắt Hồn
    assert(jsonStory.isInDialogue() == true);
    assert(jsonStory.getCurrentDialogueNode().id == "d02_start");
    assert(jsonStory.selectDialogueChoice(1) == true);
    assert(jsonStory.getCurrentDialogueNode().id == "d02_inquire");
    assert(jsonStory.selectDialogueChoice(0) == true);
    assert(jsonStory.isInDialogue() == false);

    // 7. Test DataLoader functions
    std::string itemsPath = "data/items.json";
    if (!std::ifstream(itemsPath).good()) itemsPath = "../data/items.json";
    auto items = DataLoader::loadItems(itemsPath);
    assert(!items.empty());

    std::string enemiesPath = "data/enemies.json";
    if (!std::ifstream(enemiesPath).good()) enemiesPath = "../data/enemies.json";
    auto enemies = DataLoader::loadEnemies(enemiesPath);
    assert(!enemies.empty());

    std::cout << "[PASS] All StoryGraph unit tests (including Branching Dialogue Tree & SaveManager helpers) passed successfully!\n";
    return 0;
}
