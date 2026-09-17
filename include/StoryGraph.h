#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <nlohmann/json.hpp>

enum class EventType {
    NORMAL,
    BATTLE,
    ITEM,
    SHOP,
    BOSS,
    ENDING
};

inline std::string eventTypeToString(EventType type) {
    switch (type) {
        case EventType::NORMAL: return "NORMAL";
        case EventType::BATTLE: return "BATTLE";
        case EventType::ITEM: return "ITEM";
        case EventType::SHOP: return "SHOP";
        case EventType::BOSS: return "BOSS";
        case EventType::ENDING: return "ENDING";
        default: return "NORMAL";
    }
}

inline EventType stringToEventType(const std::string& str) {
    if (str == "BATTLE") return EventType::BATTLE;
    if (str == "ITEM") return EventType::ITEM;
    if (str == "SHOP") return EventType::SHOP;
    if (str == "BOSS") return EventType::BOSS;
    if (str == "ENDING") return EventType::ENDING;
    return EventType::NORMAL;
}

struct Choice {
    std::string text;
    std::string nextNodeId;
    std::string requiredFlag;
    std::string setFlag;

    Choice(const std::string& text = "", const std::string& nextNodeId = "",
           const std::string& requiredFlag = "", const std::string& setFlag = "")
        : text(text), nextNodeId(nextNodeId), requiredFlag(requiredFlag), setFlag(setFlag) {}
};

struct StoryNode {
    std::string id;
    std::string text;
    EventType type = EventType::NORMAL;
    std::vector<Choice> choices;
    std::string enemyId;      // For BATTLE / BOSS events (e.g. "goblin", "dragon_lord")
    std::string rewardItemId; // For ITEM events
    int rewardGold = 0;

    StoryNode() = default;
    StoryNode(const std::string& id, const std::string& text, EventType type = EventType::NORMAL,
              const std::vector<Choice>& choices = {},
              const std::string& enemyId = "",
              const std::string& rewardItemId = "",
              int rewardGold = 0)
        : id(id), text(text), type(type), choices(choices),
          enemyId(enemyId), rewardItemId(rewardItemId), rewardGold(rewardGold) {}
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
    bool loadStoryGraph(const std::string& filePath = "data/story.json");
    
    StoryNode getCurrentNode() const;
    std::string getCurrentNodeId() const;
    bool selectChoice(int choiceIndex);
    bool moveToNode(const std::string& nodeId);
    
    void setFlag(const std::string& flag, bool value = true);
    bool getFlag(const std::string& flag) const;
    const std::unordered_map<std::string, bool>& getFlags() const;
    void setFlags(const std::unordered_map<std::string, bool>& flags);
    
    bool isEnding() const;
    const std::unordered_map<std::string, StoryNode>& getAllNodes() const;
};
