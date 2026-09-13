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

    // Kiểm thử nạp NPC và Dialogues từ data/story.json
    assert(startNode.npcName == "Trưởng Làng Eldrin");
    assert(startNode.dialogues.size() == 3);
    assert(startNode.dialogues[0].speaker == "Trưởng Làng Eldrin");
    assert(startNode.dialogues[1].speaker == "Người chơi");

    // Xác nhận toàn bộ 12 node đều có NPC và thoại phong phú
    const auto& allNodes = jsonStory.getAllNodes();
    assert(allNodes.size() == 12);
    for (const auto& [nodeId, storyNode] : allNodes) {
        assert(!storyNode.npcName.empty());
        assert(!storyNode.dialogues.empty());
    }

    // Kiểm thử di chuyển trực tiếp tới các node kết thúc
    jsonStory.moveToNode("node_06");
    assert(jsonStory.isEnding() == true);
    assert(jsonStory.getCurrentNode().npcName == "Thần Chết Băng Giá");

    // 3. Kiểm thử cơ chế Hội thoại rẽ nhánh (Branching Dialogue Tree)
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
    // Chọn đáp thoại [1]: Thăm dò ai phái tới -> rẽ sang d02_inquire
    assert(jsonStory.selectDialogueChoice(1) == true);
    assert(jsonStory.getCurrentDialogueNode().id == "d02_inquire");
    // Chọn đáp thoại [0]: Giao chiến -> kết thúc hội thoại, bước vào đánh quái
    assert(jsonStory.selectDialogueChoice(0) == true);
    assert(jsonStory.isInDialogue() == false);

    // Thử nghiệm nhánh hội thoại dẫn tới Hang Cổ (node_03)
    jsonStory.moveToNode("node_01");
    assert(jsonStory.isInDialogue() == true);
    // Chọn đáp thoại [1]: Hỏi về Hang Cổ Băng Giá -> rẽ sang d01_cave_info
    assert(jsonStory.selectDialogueChoice(1) == true);
    assert(jsonStory.getCurrentDialogueNode().id == "d01_cave_info");
    // Chọn đáp thoại [0]: Chọn đến Hang Cổ -> Dẫn sang node_03
    assert(jsonStory.selectDialogueChoice(0) == true);
    assert(jsonStory.getCurrentNode().id == "node_03");

    std::cout << "[PASS] All StoryGraph unit tests (including Branching Dialogue Tree) passed successfully!\n";
    return 0;
}

