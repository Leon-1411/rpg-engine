#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class EventType {
    NORMAL,
    BATTLE,
    ITEM,
    SHOP,
    ENDING
};

struct Choice {
    std::string text;
    std::string nextNodeId;
    std::string requiredFlag;
    std::string setFlag;
};

struct DialogueLine {
    std::string speaker;
    std::string text;
};

struct DialogueChoice {
    std::string text;
    std::string nextDialogueId;
    std::string nextNodeId;
    std::string requiredFlag;
    std::string setFlag;
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogueChoice> choices;
};

struct DialogueTree {
    std::string startDialogueId;
    std::unordered_map<std::string, DialogueNode> nodes;

    bool empty() const { return nodes.empty(); }
};

struct StoryNode {
    std::string id;
    std::string text;
    EventType type = EventType::NORMAL;
    std::vector<Choice> choices;

    // Thuộc tính mở rộng từ file JSON
    std::string title;
    std::string rawType;
    std::string enemyId;
    std::string onWinNodeId;
    std::string onLoseNodeId;
    std::vector<std::string> rewardItems;
    int rewardExp = 0;
    std::string requiredItem;
    std::string onPassNodeId;
    std::string onFailNodeId;
    std::string nextNodeId;

    // Thuộc tính đối thoại NPC tuyến tính (backward compatibility)
    std::string npcName;
    std::vector<DialogueLine> dialogues;

    // Cơ chế Hội thoại rẽ nhánh (Branching Dialogue Tree)
    DialogueTree dialogueTree;
};

class StoryGraph {
private:
    std::unordered_map<std::string, StoryNode> nodes;
    std::string currentNodeId;
    std::string currentDialogueId;
    std::unordered_map<std::string, bool> storyFlags;

public:
    StoryGraph();
    ~StoryGraph() = default;

    void addNode(const StoryNode& node);
    bool loadStoryGraph(const std::string& filePath);
    
    StoryNode getCurrentNode() const;
    bool selectChoice(int choiceIndex);
    bool moveToNode(const std::string& nodeId);
    
    // Quản lý hội thoại rẽ nhánh
    bool isInDialogue() const;
    DialogueNode getCurrentDialogueNode() const;
    bool selectDialogueChoice(int choiceIndex);
    void resetDialogue();

    void setFlag(const std::string& flag, bool value = true);
    bool getFlag(const std::string& flag) const;
    
    bool isEnding() const;
    size_t getNodeCount() const;
    const std::unordered_map<std::string, StoryNode>& getAllNodes() const;
};

