/**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods.
 * @author Nghĩa & Antigravity
 */

#include "StoryGraph.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

static EventType stringToEventType(const std::string& str) {
    if (str == "COMBAT" || str == "BATTLE") return EventType::BATTLE;
    if (str == "REWARD" || str == "ITEM") return EventType::ITEM;
    if (str == "SHOP") return EventType::SHOP;
    if (str == "GAME_OVER" || str == "VICTORY" || str == "ENDING") return EventType::ENDING;
    return EventType::NORMAL;
}

StoryGraph::StoryGraph() : currentNodeId("start") {
    // Default initial graph skeleton
    StoryNode startNode;
    startNode.id = "start";
    startNode.text = "You awaken in a mysterious forest glade.";
    startNode.type = EventType::NORMAL;
    startNode.choices.push_back({"Walk into the dark forest", "forest_path", "", ""});
    startNode.choices.push_back({"Follow the river trail", "river_trail", "", ""});
    nodes["start"] = startNode;
}

void StoryGraph::addNode(const StoryNode& node) {
    nodes[node.id] = node;
}

void StoryGraph::clear() {
    nodes.clear();
    currentNodeId.clear();
    storyFlags.clear();
}

const std::unordered_map<std::string, StoryNode>& StoryGraph::getAllNodes() const {
    return nodes;
}

bool StoryGraph::loadFromJsonString(const std::string& jsonContent) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonContent);
        nodes.clear();
        storyFlags.clear();

        if (j.contains("start_node_id")) {
            currentNodeId = j["start_node_id"].get<std::string>();
        } else if (j.contains("startNodeId")) {
            currentNodeId = j["startNodeId"].get<std::string>();
        } else {
            currentNodeId = "start";
        }

        if (j.contains("nodes")) {
            if (j["nodes"].is_object()) {
                for (auto it : j["nodes"].items()) {
                    std::string key = it.key();
                    const auto& nodeJson = it.value();
                    StoryNode node;
                    node.id = key;
                    if (nodeJson.contains("id")) node.id = nodeJson["id"].get<std::string>();
                    if (nodeJson.contains("title")) node.title = nodeJson["title"].get<std::string>();
                    if (nodeJson.contains("description")) node.text = nodeJson["description"].get<std::string>();
                    else if (nodeJson.contains("text")) node.text = nodeJson["text"].get<std::string>();

                    std::string typeStr = nodeJson.value("type", "STORY");
                    node.rawType = typeStr;
                    node.type = stringToEventType(typeStr);

                    if (nodeJson.contains("choices") && nodeJson["choices"].is_array()) {
                        for (auto& c : nodeJson["choices"]) {
                            Choice choice;
                            choice.text = c.value("text", "");
                            choice.nextNodeId = c.value("next_node_id", c.value("nextNodeId", ""));
                            choice.requiredFlag = c.value("required_flag", c.value("requiredFlag", ""));
                            choice.setFlag = c.value("set_flag", c.value("setFlag", ""));
                            node.choices.push_back(choice);
                        }
                    }

                    node.enemyId = nodeJson.value("enemy_id", "");
                    node.onWinNodeId = nodeJson.value("on_win_node_id", "");
                    node.onLoseNodeId = nodeJson.value("on_lose_node_id", "");
                    node.requiredItem = nodeJson.value("required_item", "");
                    node.onPassNodeId = nodeJson.value("on_pass_node_id", "");
                    node.onFailNodeId = nodeJson.value("on_fail_node_id", "");
                    node.nextNodeId = nodeJson.value("next_node_id", "");

                    if (nodeJson.contains("rewards") && nodeJson["rewards"].is_object()) {
                        auto rew = nodeJson["rewards"];
                        node.rewardExp = rew.value("exp", 0);
                        if (rew.contains("items") && rew["items"].is_array()) {
                            for (auto& item : rew["items"]) {
                                node.rewardItems.push_back(item.get<std::string>());
                            }
                        }
                    }

                    nodes[node.id] = node;
                }
            } else if (j["nodes"].is_array()) {
                for (auto& nodeJson : j["nodes"]) {
                    StoryNode node;
                    node.id = nodeJson.value("id", "");
                    node.title = nodeJson.value("title", "");
                    node.text = nodeJson.value("description", nodeJson.value("text", ""));
                    std::string typeStr = nodeJson.value("type", "NORMAL");
                    node.rawType = typeStr;
                    node.type = stringToEventType(typeStr);

                    if (nodeJson.contains("choices") && nodeJson["choices"].is_array()) {
                        for (auto& c : nodeJson["choices"]) {
                            Choice choice;
                            choice.text = c.value("text", "");
                            choice.nextNodeId = c.value("next_node_id", c.value("nextNodeId", ""));
                            choice.requiredFlag = c.value("required_flag", c.value("requiredFlag", ""));
                            choice.setFlag = c.value("set_flag", c.value("setFlag", ""));
                            node.choices.push_back(choice);
                        }
                    }
                    nodes[node.id] = node;
                }
            }
        }

        if (nodes.empty()) {
            return false;
        }

        if (nodes.find(currentNodeId) == nodes.end() && !nodes.empty()) {
            currentNodeId = nodes.begin()->first;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[StoryGraph] JSON Parse error: " << e.what() << "\n";
        return false;
    }
}

bool StoryGraph::loadStoryGraph(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[StoryGraph] Error: Could not open file " << filePath << "\n";
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    return loadFromJsonString(content);
}

StoryNode StoryGraph::getCurrentNode() const {
    auto it = nodes.find(currentNodeId);
    if (it != nodes.end()) {
        return it->second;
    }
    return {};
}

bool StoryGraph::selectChoice(int choiceIndex) {
    auto node = getCurrentNode();
    if (choiceIndex >= 0 && choiceIndex < static_cast<int>(node.choices.size())) {
        const Choice& choice = node.choices[choiceIndex];
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

bool StoryGraph::isEnding() const {
    return getCurrentNode().type == EventType::ENDING;
}

size_t StoryGraph::getNodeCount() const {
    return nodes.size();
}
