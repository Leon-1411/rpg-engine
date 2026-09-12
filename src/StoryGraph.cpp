/**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods.
 * @author Nghĩa
 */

#include "StoryGraph.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

static EventType stringToEventType(const std::string& str) {
    if (str == "BATTLE") return EventType::BATTLE;
    if (str == "ITEM") return EventType::ITEM;
    if (str == "SHOP") return EventType::SHOP;
    if (str == "ENDING") return EventType::ENDING;
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
        std::unordered_map<std::string, StoryNode> parsedNodes;
        std::string startId;

        if (j.contains("startNodeId") && j["startNodeId"].is_string()) {
            startId = j["startNodeId"].get<std::string>();
        }

        if (j.contains("nodes") && j["nodes"].is_array()) {
            for (const auto& item : j["nodes"]) {
                StoryNode node;
                if (!item.contains("id") || !item["id"].is_string()) continue;
                node.id = item["id"].get<std::string>();
                if (item.contains("text") && item["text"].is_string()) {
                    node.text = item["text"].get<std::string>();
                }
                if (item.contains("type") && item["type"].is_string()) {
                    node.type = stringToEventType(item["type"].get<std::string>());
                } else {
                    node.type = EventType::NORMAL;
                }

                if (item.contains("choices") && item["choices"].is_array()) {
                    for (const auto& ch : item["choices"]) {
                        Choice choice;
                        if (ch.contains("text") && ch["text"].is_string()) {
                            choice.text = ch["text"].get<std::string>();
                        }
                        if (ch.contains("nextNodeId") && ch["nextNodeId"].is_string()) {
                            choice.nextNodeId = ch["nextNodeId"].get<std::string>();
                        }
                        if (ch.contains("requiredFlag") && ch["requiredFlag"].is_string()) {
                            choice.requiredFlag = ch["requiredFlag"].get<std::string>();
                        }
                        if (ch.contains("setFlag") && ch["setFlag"].is_string()) {
                            choice.setFlag = ch["setFlag"].get<std::string>();
                        }
                        node.choices.push_back(choice);
                    }
                }
                parsedNodes[node.id] = node;
                if (startId.empty()) {
                    startId = node.id;
                }
            }
        }

        if (parsedNodes.empty()) {
            std::cerr << "[StoryGraph] Cảnh báo: Không tìm thấy node hợp lệ trong dữ liệu JSON.\n";
            return false;
        }

        nodes = std::move(parsedNodes);
        if (!startId.empty() && nodes.find(startId) != nodes.end()) {
            currentNodeId = startId;
        } else if (!nodes.empty()) {
            currentNodeId = nodes.begin()->first;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[StoryGraph] Lỗi cú pháp JSON: " << e.what() << "\n";
        return false;
    }
}

bool StoryGraph::loadStoryGraph(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[StoryGraph] Lỗi: Không thể mở file " << filePath << "\n";
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
