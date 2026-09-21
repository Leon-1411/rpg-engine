#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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
    int requiredGold;
    int goldCost;
    std::string requiredHeroClass;
    std::vector<std::string> requiredItems;

    Choice() : requiredGold(0), goldCost(0) {}
    Choice(const std::string& text, const std::string& nextNodeId,
           const std::string& reqFlag = "", const std::string& setFlag = "",
           int reqGold = 0, int goldCost = 0,
           const std::string& reqClass = "", const std::vector<std::string>& reqItems = {})
        : text(text), nextNodeId(nextNodeId), requiredFlag(reqFlag), setFlag(setFlag),
          requiredGold(reqGold), goldCost(goldCost), requiredHeroClass(reqClass), requiredItems(reqItems) {}
};

struct DialogueLine {
    std::string speaker;
    std::string text;

    DialogueLine() = default;
    DialogueLine(const std::string& speaker, const std::string& text) : speaker(speaker), text(text) {}
};

struct DialogueChoice {
    std::string text;
    std::string nextDialogueId;
    std::string nextNodeId;
    std::string requiredFlag;
    std::string setFlag;
    int requiredGold;
    int goldCost;

    DialogueChoice() : requiredGold(0), goldCost(0) {}
    DialogueChoice(const std::string& text, const std::string& nextDialId = "", const std::string& nextNId = "",
                   const std::string& reqFlag = "", const std::string& setFlag = "",
                   int reqGold = 0, int goldCost = 0)
        : text(text), nextDialogueId(nextDialId), nextNodeId(nextNId),
          requiredFlag(reqFlag), setFlag(setFlag), requiredGold(reqGold), goldCost(goldCost) {}
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string text;
    std::vector<DialogueChoice> choices;

    DialogueNode() = default;
    DialogueNode(const std::string& id, const std::string& speaker, const std::string& text, const std::vector<DialogueChoice>& choices = {})
        : id(id), speaker(speaker), text(text), choices(choices) {}
};

struct DialogueTree {
    std::string startDialogueId;
    std::unordered_map<std::string, DialogueNode> nodes;

    bool empty() const { return nodes.empty(); }
};

struct StoryNode {
    std::string id;
    std::string text;
    EventType type;
    std::vector<Choice> choices;

    // Thuoc tinh mo rong tu file JSON
    std::string title;
    std::string rawType;
    std::string enemyId;
    std::string onWinNodeId;
    std::string onLoseNodeId;
    std::vector<std::string> rewardItems;
    int rewardExp;
    int rewardGold;
    std::string requiredItem;
    std::vector<std::string> requiredItems;
    std::string requiredHeroClass;
    std::string onPassNodeId;
    std::string onFailNodeId;
    std::string nextNodeId;

    // Thuoc tinh doi thoai NOC tuyen tinh (backward compatibility)
    std::string npcName;
    std::vector<DialogueLine> dialogues;

    // Co che Hoi thoai re nhanh (Branching Dialogue Tree)
    DialogueTree dialogueTree;

    StoryNode() : type(EventType::NORMAL), rewardExp(0), rewardGold(0) {}
    StoryNode(const std::string& id, const std::string& text, EventType type = EventType::NORMAL, const std::vector<Choice>& choices = {})
        : id(id), text(text), type(type), choices(choices), rewardExp(0), rewardGold(0) {}
};

class StoryGraph {
private:
    std::unordered_map<std::string, StoryNode> nodes;
    std::string currentNodeId;
    std::string currentDialogueId;
    std::unordered_map<std::string, bool> storyFlags;
    // Quan ly tien do cot truyen & Mo khoa Endings (Story Progress Codex)
    std::vector<std::string> visitedNodes;

public:
    StoryGraph();
    ~StoryGraph() = default;

    void addNode(const StoryNode& node);
    bool loadStoryGraph(const std::string& filePath);
    bool loadFromJsonString(const std::string& jsonContent);
    void clear();
    const std::unordered_map<std::string, StoryNode>& getAllNodes() const;
    
    StoryNode getCurrentNode() const;
    std::string getCurrentNodeId() const;
    void setCurrentNodeId(const std::string& nodeId);
    bool selectChoice(int choiceIndex);
    bool moveToNode(const std::string& nodeId);
    
    bool canSelectChoice(int choiceIndex) const;
    bool canSelectDialogueChoice(int choiceIndex) const;

    // Quan ly hoi thoai re nhanh
    bool isInDialogue() const;
    DialogueNode getCurrentDialogueNode() const;
    bool selectDialogueChoice(int choiceIndex);
    void resetDialogue();

    void setFlag(const std::string& flag, bool value = true);
    bool getFlag(const std::string& flag) const;
    const std::unordered_map<std::string, bool>& getStoryFlags() const;
    void setStoryFlags(const std::unordered_map<std::string, bool>& flags);
    void clearStoryFlags();
    
    bool isEnding() const;
    bool isGameOverNode() const;
    size_t getNodeCount() const;

    // Kiem tra tinh toan ven do thi va phat hien cut duong (Dead-end / Broken links)
    bool validateGraph(std::vector<std::string>& errors) const;

    // Tien do choi, Endings va Node chua mo khoa (Progress Tracking & Codex)
    void markNodeVisited(const std::string& nodeId);
    bool isNodeVisited(const std::string& nodeId) const;
    const std::vector<std::string>& getVisitedNodes() const;
    void setVisitedNodes(const std::vector<std::string>& visited);
    std::vector<std::string> getLockedNodes() const;
    std::vector<std::string> getAllEndings() const;
    std::vector<std::string> getDiscoveredEndings() const;
    float getExplorationPercentage() const;
    void printStoryProgress() const;
};
