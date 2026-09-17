/**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods with full JSON data loading and branching flags.
 */

#include "StoryGraph.h"
#include <fstream>
#include <iostream>

StoryGraph::StoryGraph() : currentNodeId("start") {
    // Attempt to load from default story file
    if (!loadStoryGraph("data/story.json")) {
        // Default initial fallback graph if data file is absent
        StoryNode startNode;
        startNode.id = "start";
        startNode.text = "You awaken in a mysterious forest glade.";
        startNode.type = EventType::NORMAL;
        startNode.choices.push_back({"Walk into the dark forest", "forest_path", "", ""});
        startNode.choices.push_back({"Follow the river trail", "river_trail", "", ""});
        nodes["start"] = startNode;
    }
}

void StoryGraph::addNode(const StoryNode& node) {
    nodes[node.id] = node;
}

bool StoryGraph::loadStoryGraph(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json data;
        file >> data;

        nodes.clear();
        for (auto& [nodeId, val] : data.items()) {
            StoryNode node;
            node.id = nodeId;
            node.text = val.value("text", "");
            node.type = stringToEventType(val.value("type", "NORMAL"));
            node.enemyId = val.value("enemyId", "");
            node.rewardItemId = val.value("rewardItemId", "");
            node.rewardGold = val.value("rewardGold", 0);

            if (val.contains("choices") && val["choices"].is_array()) {
                for (const auto& cVal : val["choices"]) {
                    Choice choice;
                    choice.text = cVal.value("text", "");
                    choice.nextNodeId = cVal.value("nextNodeId", "");
                    choice.requiredFlag = cVal.value("requiredFlag", "");
                    choice.setFlag = cVal.value("setFlag", "");
                    node.choices.push_back(choice);
                }
            }

            nodes[nodeId] = node;
        }

        if (nodes.find("start") != nodes.end()) {
            currentNodeId = "start";
        } else if (!nodes.empty()) {
            currentNodeId = nodes.begin()->first;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[StoryGraph] Error loading story JSON: " << e.what() << "\n";
        return false;
    }
}

StoryNode StoryGraph::getCurrentNode() const {
    auto it = nodes.find(currentNodeId);
    if (it != nodes.end()) {
        return it->second;
    }
    return {};
}

std::string StoryGraph::getCurrentNodeId() const {
    return currentNodeId;
}

bool StoryGraph::selectChoice(int choiceIndex) {
    auto node = getCurrentNode();
    if (choiceIndex >= 0 && choiceIndex < static_cast<int>(node.choices.size())) {
        const Choice& choice = node.choices[choiceIndex];

        // Check required flag if specified
        if (!choice.requiredFlag.empty()) {
            if (!getFlag(choice.requiredFlag)) {
                return false;
            }
        }

        if (!choice.setFlag.empty()) {
            setFlag(choice.setFlag, true);
        }
        return moveToNode(choice.nextNodeId);
    }
    return false;
}

bool StoryGraph::moveToNode(const std::string& nodeId) {
    if (nodes.find(nodeId) != nodes.end()) {
        currentNodeId = nodeId;
        return true;
    }
    return false;
}

void StoryGraph::setFlag(const std::string& flag, bool value) {
    storyFlags[flag] = value;
}

bool StoryGraph::getFlag(const std::string& flag) const {
    auto it = storyFlags.find(flag);
    if (it != storyFlags.end()) return it->second;
    return false;
}

const std::unordered_map<std::string, bool>& StoryGraph::getFlags() const {
    return storyFlags;
}

void StoryGraph::setFlags(const std::unordered_map<std::string, bool>& flags) {
    storyFlags = flags;
}

bool StoryGraph::isEnding() const {
    return getCurrentNode().type == EventType::ENDING;
}

const std::unordered_map<std::string, StoryNode>& StoryGraph::getAllNodes() const {
    return nodes;
}
