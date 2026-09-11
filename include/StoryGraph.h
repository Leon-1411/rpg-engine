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
};

class StoryGraph {
private:
    std::unordered_map<std::string, StoryNode> nodes;
    std::string currentNodeId;
    std::unordered_map<std::string, bool> storyFlags;

public:
    StoryGraph();
    ~StoryGraph() = default;

    void addNode(const StoryNode& node);
    bool loadStoryGraph(const std::string& filePath);
    
    StoryNode getCurrentNode() const;
    bool selectChoice(int choiceIndex);
    bool moveToNode(const std::string& nodeId);
    
    void setFlag(const std::string& flag, bool value = true);
    bool getFlag(const std::string& flag) const;
    
    bool isEnding() const;
    size_t getNodeCount() const;
    const std::unordered_map<std::string, StoryNode>& getAllNodes() const;
};

