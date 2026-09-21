/**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods.
 * @author Nghĩa & Antigravity
 */

#include "StoryGraph.h"
#include "ui/ConsoleUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
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
    markNodeVisited("start");
}

void StoryGraph::addNode(const StoryNode& node) {
    nodes[node.id] = node;
}

void StoryGraph::clear() {
    nodes.clear();
    currentNodeId.clear();
    currentDialogueId.clear();
    storyFlags.clear();
    visitedNodes.clear();
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
                for (auto item : j["nodes"].items()) {
                    std::string key = item.key();
                    const auto& nodeJson = item.value();
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
                            choice.requiredGold = c.value("required_gold", c.value("requiredGold", 0));
                            choice.goldCost = c.value("gold_cost", c.value("goldCost", 0));
                            choice.requiredHeroClass = c.value("requiredHeroClass", c.value("required_class", ""));
                            if (c.contains("requiredItems") && c["requiredItems"].is_array()) {
                                for (const auto& item : c["requiredItems"]) {
                                    choice.requiredItems.push_back(item.get<std::string>());
                                }
                            } else if (c.contains("requiredItem") && c["requiredItem"].is_string()) {
                                choice.requiredItems.push_back(c["requiredItem"].get<std::string>());
                            }
                            node.choices.push_back(choice);
                        }
                    }

                    node.enemyId = nodeJson.value("enemy_id", "");
                    node.onWinNodeId = nodeJson.value("on_win_node_id", "");
                    node.onLoseNodeId = nodeJson.value("on_lose_node_id", "");
                    node.requiredItem = nodeJson.value("required_item", "");
                    node.requiredHeroClass = nodeJson.value("required_hero_class", "");
                    if (nodeJson.contains("required_items") && nodeJson["required_items"].is_array()) {
                        for (auto& itm : nodeJson["required_items"]) {
                            node.requiredItems.push_back(itm.get<std::string>());
                        }
                    } else if (!node.requiredItem.empty()) {
                        node.requiredItems.push_back(node.requiredItem);
                    }
                    node.onPassNodeId = nodeJson.value("on_pass_node_id", "");
                    node.onFailNodeId = nodeJson.value("on_fail_node_id", "");
                    node.nextNodeId = nodeJson.value("next_node_id", "");

                    if (nodeJson.contains("rewards") && nodeJson["rewards"].is_object()) {
                        auto rew = nodeJson["rewards"];
                        node.rewardExp = rew.value("exp", 0);
                        node.rewardGold = rew.value("gold", rew.value("rewardGold", 0));
                        if (rew.contains("items") && rew["items"].is_array()) {
                            for (auto& item : rew["items"]) {
                                node.rewardItems.push_back(item.get<std::string>());
                            }
                        }
                    }

                    if (nodeJson.contains("npc")) {
                        if (nodeJson["npc"].is_string()) {
                            node.npcName = nodeJson["npc"].get<std::string>();
                        } else if (nodeJson["npc"].is_object()) {
                            node.npcName = nodeJson["npc"].value("name", "");
                        }
                    }

                    if (nodeJson.contains("dialogues") && nodeJson["dialogues"].is_array()) {
                        for (auto& d : nodeJson["dialogues"]) {
                            DialogueLine line;
                            if (d.is_object()) {
                                line.speaker = d.value("speaker", node.npcName.empty() ? "NPC" : node.npcName);
                                line.text = d.value("text", "");
                            } else if (d.is_string()) {
                                line.speaker = node.npcName.empty() ? "NPC" : node.npcName;
                                line.text = d.get<std::string>();
                            }
                            node.dialogues.push_back(line);
                        }
                    }

                    if (nodeJson.contains("dialogue_tree") && nodeJson["dialogue_tree"].is_object()) {
                        auto dtJson = nodeJson["dialogue_tree"];
                        node.dialogueTree.startDialogueId = dtJson.value("start_dialogue_id", dtJson.value("start_id", ""));
                        if (dtJson.contains("nodes") && dtJson["nodes"].is_object()) {
                            for (auto dItem : dtJson["nodes"].items()) {
                                std::string dKey = dItem.key();
                                const auto& dVal = dItem.value();
                                DialogueNode dn;
                                dn.id = dKey;
                                if (dVal.contains("id")) dn.id = dVal["id"].get<std::string>();
                                dn.speaker = dVal.value("speaker", node.npcName.empty() ? "NPC" : node.npcName);
                                dn.text = dVal.value("text", "");
                                if (dVal.contains("choices") && dVal["choices"].is_array()) {
                                    for (auto& ch : dVal["choices"]) {
                                        DialogueChoice dc;
                                        dc.text = ch.value("text", "");
                                        dc.nextDialogueId = ch.value("next_dialogue_id", ch.value("nextDialogueId", ""));
                                        dc.nextNodeId = ch.value("next_node_id", ch.value("nextNodeId", ""));
                                        dc.requiredFlag = ch.value("required_flag", ch.value("requiredFlag", ""));
                                        dc.setFlag = ch.value("set_flag", ch.value("setFlag", ""));
                                        dc.requiredGold = ch.value("required_gold", ch.value("requiredGold", 0));
                                        dc.goldCost = ch.value("gold_cost", ch.value("goldCost", 0));
                                        dn.choices.push_back(dc);
                                    }
                                }
                                node.dialogueTree.nodes[dn.id] = dn;
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

                    node.enemyId = nodeJson.value("enemy_id", "");
                    node.onWinNodeId = nodeJson.value("on_win_node_id", "");
                    node.onLoseNodeId = nodeJson.value("on_lose_node_id", "");
                    node.requiredItem = nodeJson.value("required_item", "");
                    node.requiredHeroClass = nodeJson.value("required_hero_class", "");
                    if (nodeJson.contains("required_items") && nodeJson["required_items"].is_array()) {
                        for (auto& itm : nodeJson["required_items"]) {
                            node.requiredItems.push_back(itm.get<std::string>());
                        }
                    } else if (!node.requiredItem.empty()) {
                        node.requiredItems.push_back(node.requiredItem);
                    }
                    node.onPassNodeId = nodeJson.value("on_pass_node_id", "");
                    node.onFailNodeId = nodeJson.value("on_fail_node_id", "");
                    node.nextNodeId = nodeJson.value("next_node_id", "");

                    if (nodeJson.contains("rewards") && nodeJson["rewards"].is_object()) {
                        auto rew = nodeJson["rewards"];
                        node.rewardExp = rew.value("exp", 0);
                        node.rewardGold = rew.value("gold", rew.value("rewardGold", 0));
                        if (rew.contains("items") && rew["items"].is_array()) {
                            for (auto& item : rew["items"]) {
                                node.rewardItems.push_back(item.get<std::string>());
                            }
                        }
                    }

                    if (nodeJson.contains("choices") && nodeJson["choices"].is_array()) {
                        for (auto& c : nodeJson["choices"]) {
                            Choice choice;
                            choice.text = c.value("text", "");
                            choice.nextNodeId = c.value("next_node_id", c.value("nextNodeId", ""));
                            choice.requiredFlag = c.value("required_flag", c.value("requiredFlag", ""));
                            choice.setFlag = c.value("set_flag", c.value("setFlag", ""));
                            choice.requiredGold = c.value("required_gold", c.value("requiredGold", 0));
                            choice.goldCost = c.value("gold_cost", c.value("goldCost", 0));
                            choice.requiredHeroClass = c.value("requiredHeroClass", c.value("required_class", ""));
                            if (c.contains("requiredItems") && c["requiredItems"].is_array()) {
                                for (const auto& item : c["requiredItems"]) {
                                    choice.requiredItems.push_back(item.get<std::string>());
                                }
                            } else if (c.contains("requiredItem") && c["requiredItem"].is_string()) {
                                choice.requiredItems.push_back(c["requiredItem"].get<std::string>());
                            }
                            node.choices.push_back(choice);
                        }
                    }

                    if (nodeJson.contains("npc")) {
                        if (nodeJson["npc"].is_string()) {
                            node.npcName = nodeJson["npc"].get<std::string>();
                        } else if (nodeJson["npc"].is_object()) {
                            node.npcName = nodeJson["npc"].value("name", "");
                        }
                    }

                    if (nodeJson.contains("dialogues") && nodeJson["dialogues"].is_array()) {
                        for (auto& d : nodeJson["dialogues"]) {
                            DialogueLine line;
                            if (d.is_object()) {
                                line.speaker = d.value("speaker", node.npcName.empty() ? "NPC" : node.npcName);
                                line.text = d.value("text", "");
                            } else if (d.is_string()) {
                                line.speaker = node.npcName.empty() ? "NPC" : node.npcName;
                                line.text = d.get<std::string>();
                            }
                            node.dialogues.push_back(line);
                        }
                    }

                    if (nodeJson.contains("dialogue_tree") && nodeJson["dialogue_tree"].is_object()) {
                        auto dtJson = nodeJson["dialogue_tree"];
                        node.dialogueTree.startDialogueId = dtJson.value("start_dialogue_id", dtJson.value("start_id", ""));
                        if (dtJson.contains("nodes") && dtJson["nodes"].is_object()) {
                            for (auto dItem : dtJson["nodes"].items()) {
                                std::string dKey = dItem.key();
                                const auto& dVal = dItem.value();
                                DialogueNode dn;
                                dn.id = dKey;
                                if (dVal.contains("id")) dn.id = dVal["id"].get<std::string>();
                                dn.speaker = dVal.value("speaker", node.npcName.empty() ? "NPC" : node.npcName);
                                dn.text = dVal.value("text", "");
                                if (dVal.contains("choices") && dVal["choices"].is_array()) {
                                    for (auto& ch : dVal["choices"]) {
                                        DialogueChoice dc;
                                        dc.text = ch.value("text", "");
                                        dc.nextDialogueId = ch.value("next_dialogue_id", ch.value("nextDialogueId", ""));
                                        dc.nextNodeId = ch.value("next_node_id", ch.value("nextNodeId", ""));
                                        dc.requiredFlag = ch.value("required_flag", ch.value("requiredFlag", ""));
                                        dc.setFlag = ch.value("set_flag", ch.value("setFlag", ""));
                                        dc.requiredGold = ch.value("required_gold", ch.value("requiredGold", 0));
                                        dc.goldCost = ch.value("gold_cost", ch.value("goldCost", 0));
                                        dn.choices.push_back(dc);
                                    }
                                }
                                node.dialogueTree.nodes[dn.id] = dn;
                            }
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

        markNodeVisited(currentNodeId);
        resetDialogue();
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
    bool ok = loadFromJsonString(content);
    if (ok) {
        std::cout << "[StoryGraph] Successfully loaded " << nodes.size() << " nodes from " << filePath << ".\n";
    }
    return ok;
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

void StoryGraph::setCurrentNodeId(const std::string& nodeId) {
    currentNodeId = nodeId;
    markNodeVisited(nodeId);
    resetDialogue();
}

bool StoryGraph::canSelectChoice(int choiceIndex) const {
    auto node = getCurrentNode();
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(node.choices.size())) {
        return false;
    }
    const Choice& choice = node.choices[choiceIndex];
    if (!choice.requiredFlag.empty() && !getFlag(choice.requiredFlag)) {
        return false;
    }
    return true;
}

bool StoryGraph::selectChoice(int choiceIndex) {
    if (!canSelectChoice(choiceIndex)) {
        return false;
    }
    auto node = getCurrentNode();
    const Choice& choice = node.choices[choiceIndex];
    if (!choice.setFlag.empty()) {
        setFlag(choice.setFlag, true);
    }
    return moveToNode(choice.nextNodeId);
}

bool StoryGraph::moveToNode(const std::string& nodeId) {
    if (nodes.find(nodeId) != nodes.end()) {
        currentNodeId = nodeId;
        markNodeVisited(nodeId);
        resetDialogue();
        return true;
    }
    return false;
}

void StoryGraph::resetDialogue() {
    auto it = nodes.find(currentNodeId);
    if (it != nodes.end() && !it->second.dialogueTree.empty()) {
        currentDialogueId = it->second.dialogueTree.startDialogueId;
    } else {
        currentDialogueId.clear();
    }
}

bool StoryGraph::isInDialogue() const {
    if (currentDialogueId.empty()) return false;
    auto it = nodes.find(currentNodeId);
    if (it != nodes.end()) {
        return it->second.dialogueTree.nodes.find(currentDialogueId) != it->second.dialogueTree.nodes.end();
    }
    return false;
}

DialogueNode StoryGraph::getCurrentDialogueNode() const {
    auto it = nodes.find(currentNodeId);
    if (it != nodes.end()) {
        auto dIt = it->second.dialogueTree.nodes.find(currentDialogueId);
        if (dIt != it->second.dialogueTree.nodes.end()) {
            return dIt->second;
        }
    }
    return {};
}

bool StoryGraph::canSelectDialogueChoice(int choiceIndex) const {
    if (!isInDialogue()) return false;
    DialogueNode dNode = getCurrentDialogueNode();
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(dNode.choices.size())) {
        return false;
    }
    const DialogueChoice& choice = dNode.choices[choiceIndex];
    if (!choice.requiredFlag.empty() && !getFlag(choice.requiredFlag)) {
        return false;
    }
    return true;
}

bool StoryGraph::selectDialogueChoice(int choiceIndex) {
    if (!canSelectDialogueChoice(choiceIndex)) return false;
    DialogueNode dNode = getCurrentDialogueNode();
    const DialogueChoice& choice = dNode.choices[choiceIndex];
    if (!choice.setFlag.empty()) {
        setFlag(choice.setFlag, true);
    }

    // Nếu đáp thoại dẫn tiếp sang nhánh thoại kế tiếp của NPC
    if (!choice.nextDialogueId.empty()) {
        auto it = nodes.find(currentNodeId);
        if (it != nodes.end() && it->second.dialogueTree.nodes.find(choice.nextDialogueId) != it->second.dialogueTree.nodes.end()) {
            currentDialogueId = choice.nextDialogueId;
            return true;
        }
    }

    // Nếu đáp thoại kết thúc hội thoại và dẫn sang Story Node tiếp theo
    if (!choice.nextNodeId.empty()) {
        return moveToNode(choice.nextNodeId);
    }

    // Không có nextDialogueId lẫn nextNodeId: kết thúc cây thoại tại node hiện tại
    currentDialogueId.clear();
    return true;
}

void StoryGraph::setFlag(const std::string& flag, bool value) {
    storyFlags[flag] = value;
}

bool StoryGraph::getFlag(const std::string& flag) const {
    auto it = storyFlags.find(flag);
    if (it != storyFlags.end()) return it->second;
    return false;
}

const std::unordered_map<std::string, bool>& StoryGraph::getStoryFlags() const {
    return storyFlags;
}

void StoryGraph::setStoryFlags(const std::unordered_map<std::string, bool>& flags) {
    storyFlags = flags;
}

void StoryGraph::clearStoryFlags() {
    storyFlags.clear();
}

bool StoryGraph::isEnding() const {
    auto node = getCurrentNode();
    return node.type == EventType::ENDING || node.rawType == "ENDING" || node.id == "GameOver";
}

bool StoryGraph::isGameOverNode() const {
    auto node = getCurrentNode();
    return node.id == "GameOver" || node.rawType == "GAME_OVER";
}

size_t StoryGraph::getNodeCount() const {
    return nodes.size();
}

bool StoryGraph::validateGraph(std::vector<std::string>& errors) const {
    errors.clear();
    if (nodes.empty()) {
        errors.push_back("Graph is empty, no nodes loaded.");
        return false;
    }

    if (nodes.find(currentNodeId) == nodes.end()) {
        errors.push_back("Start node '" + currentNodeId + "' not found in graph.");
    }

    for (const auto& pair : nodes) {
        const std::string& id = pair.first;
        const StoryNode& node = pair.second;

        // 1. Kiem tra cac choices thong thuong
        for (size_t cIdx = 0; cIdx < node.choices.size(); ++cIdx) {
            const auto& ch = node.choices[cIdx];
            if (ch.nextNodeId.empty()) {
                errors.push_back("Node '" + id + "' choice #" + std::to_string(cIdx + 1) + " ('" + ch.text + "') has empty nextNodeId.");
            } else if (nodes.find(ch.nextNodeId) == nodes.end()) {
                errors.push_back("Node '" + id + "' choice #" + std::to_string(cIdx + 1) + " ('" + ch.text + "') links to non-existent nextNodeId: '" + ch.nextNodeId + "'.");
            }
        }

        // 2. Kiem tra Dialogue Tree
        if (!node.dialogueTree.empty()) {
            if (!node.dialogueTree.startDialogueId.empty() && 
                node.dialogueTree.nodes.find(node.dialogueTree.startDialogueId) == node.dialogueTree.nodes.end()) {
                errors.push_back("Node '" + id + "' has invalid start_dialogue_id: '" + node.dialogueTree.startDialogueId + "'.");
            }

            for (const auto& dPair : node.dialogueTree.nodes) {
                const std::string& dId = dPair.first;
                const DialogueNode& dn = dPair.second;
                for (size_t dcIdx = 0; dcIdx < dn.choices.size(); ++dcIdx) {
                    const auto& dChoice = dn.choices[dcIdx];
                    if (!dChoice.nextDialogueId.empty() && node.dialogueTree.nodes.find(dChoice.nextDialogueId) == node.dialogueTree.nodes.end()) {
                        errors.push_back("Node '" + id + "' dialogue '" + dId + "' choice #" + std::to_string(dcIdx + 1) + " links to non-existent nextDialogueId: '" + dChoice.nextDialogueId + "'.");
                    }
                    if (!dChoice.nextNodeId.empty() && nodes.find(dChoice.nextNodeId) == nodes.end()) {
                        errors.push_back("Node '" + id + "' dialogue '" + dId + "' choice #" + std::to_string(dcIdx + 1) + " links to non-existent nextNodeId: '" + dChoice.nextNodeId + "'.");
                    }
                }
            }
        }

        // 3. Kiem tra cac lien ket dac biet
        if (!node.onWinNodeId.empty() && nodes.find(node.onWinNodeId) == nodes.end()) {
            errors.push_back("Node '" + id + "' onWinNodeId links to non-existent node: '" + node.onWinNodeId + "'.");
        }
        if (!node.onLoseNodeId.empty() && nodes.find(node.onLoseNodeId) == nodes.end()) {
            errors.push_back("Node '" + id + "' onLoseNodeId links to non-existent node: '" + node.onLoseNodeId + "'.");
        }
        if (!node.onPassNodeId.empty() && nodes.find(node.onPassNodeId) == nodes.end()) {
            errors.push_back("Node '" + id + "' onPassNodeId links to non-existent node: '" + node.onPassNodeId + "'.");
        }
        if (!node.onFailNodeId.empty() && nodes.find(node.onFailNodeId) == nodes.end()) {
            errors.push_back("Node '" + id + "' onFailNodeId links to non-existent node: '" + node.onFailNodeId + "'.");
        }
        if (!node.nextNodeId.empty() && nodes.find(node.nextNodeId) == nodes.end()) {
            errors.push_back("Node '" + id + "' nextNodeId links to non-existent node: '" + node.nextNodeId + "'.");
        }

        // 4. Kiem tra Dead-end (Cut duong)
        bool isEndingNode = (node.type == EventType::ENDING || node.rawType == "ENDING" || id == "GameOver");
        if (!isEndingNode) {
            bool hasOutgoing = !node.choices.empty() ||
                               !node.nextNodeId.empty() ||
                               !node.onPassNodeId.empty() ||
                               !node.onWinNodeId.empty();

            if (!hasOutgoing && !node.dialogueTree.empty()) {
                for (const auto& dPair : node.dialogueTree.nodes) {
                    for (const auto& dChoice : dPair.second.choices) {
                        if (!dChoice.nextNodeId.empty()) {
                            hasOutgoing = true;
                            break;
                        }
                    }
                    if (hasOutgoing) break;
                }
            }

            if (!hasOutgoing) {
                errors.push_back("Dead-end detected: Node '" + id + "' is not an ENDING node but has no outgoing transitions!");
            }
        }
    }

    return errors.empty();
}

void StoryGraph::markNodeVisited(const std::string& nodeId) {
    if (nodeId.empty()) return;
    if (std::find(visitedNodes.begin(), visitedNodes.end(), nodeId) == visitedNodes.end()) {
        visitedNodes.push_back(nodeId);
    }
}

bool StoryGraph::isNodeVisited(const std::string& nodeId) const {
    return std::find(visitedNodes.begin(), visitedNodes.end(), nodeId) != visitedNodes.end();
}

const std::vector<std::string>& StoryGraph::getVisitedNodes() const {
    return visitedNodes;
}

void StoryGraph::setVisitedNodes(const std::vector<std::string>& visited) {
    visitedNodes = visited;
}

std::vector<std::string> StoryGraph::getLockedNodes() const {
    std::vector<std::string> locked;
    for (const auto& pair : nodes) {
        if (!isNodeVisited(pair.first)) {
            locked.push_back(pair.first);
        }
    }
    std::sort(locked.begin(), locked.end());
    return locked;
}

std::vector<std::string> StoryGraph::getAllEndings() const {
    std::vector<std::string> endings;
    for (const auto& pair : nodes) {
        const auto& n = pair.second;
        if (n.type == EventType::ENDING || n.rawType == "ENDING" || n.id.rfind("End", 0) == 0) {
            endings.push_back(pair.first);
        }
    }
    std::sort(endings.begin(), endings.end());
    return endings;
}

std::vector<std::string> StoryGraph::getDiscoveredEndings() const {
    std::vector<std::string> discovered;
    for (const auto& endId : getAllEndings()) {
        if (isNodeVisited(endId)) {
            discovered.push_back(endId);
        }
    }
    return discovered;
}

float StoryGraph::getExplorationPercentage() const {
    if (nodes.empty()) return 0.0f;
    return (static_cast<float>(visitedNodes.size()) / static_cast<float>(nodes.size())) * 100.0f;
}

void StoryGraph::printStoryProgress() const {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TIẾN ĐỘ KHÁM PHÁ CỐT TRUYỆN (FRACTURED CROWN CODEX)", 70, ConsoleUI::Colors::BRIGHT_YELLOW);

    size_t totalNodes = nodes.size();
    size_t visitedCount = visitedNodes.size();
    float percent = getExplorationPercentage();

    std::cout << "\n";
    std::cout << "  • Tổng số địa điểm / nút cốt truyện: " 
              << ConsoleUI::colorize(std::to_string(totalNodes), ConsoleUI::Colors::BRIGHT_WHITE) << "\n";
    std::cout << "  • Số địa điểm đã khám phá: " 
              << ConsoleUI::colorize(std::to_string(visitedCount), ConsoleUI::Colors::BRIGHT_GREEN) << "\n";
    std::cout << "  • Số địa điểm chưa mở khóa: " 
              << ConsoleUI::colorize(std::to_string(totalNodes >= visitedCount ? totalNodes - visitedCount : 0), ConsoleUI::Colors::BRIGHT_RED) << "\n";

    std::cout << "\n";
    ConsoleUI::printProgressBar("  Tỷ lệ hoàn thành cốt truyện", static_cast<int>(visitedCount), static_cast<int>(totalNodes), 30, ConsoleUI::Colors::BRIGHT_CYAN);
    std::cout << "  (Khám phá được: " << std::fixed << std::setprecision(1) << percent << "%)\n\n";

    ConsoleUI::printDivider('=', 70, ConsoleUI::Colors::CYAN);
    std::cout << ConsoleUI::colorize("  DANH SÁCH CÁC KẾT CỤC (FRACTURED CROWN ENDINGS):", ConsoleUI::Colors::BRIGHT_MAGENTA) << "\n";
    ConsoleUI::printDivider('-', 70, ConsoleUI::Colors::DIM);

    std::vector<std::string> allEndings = getAllEndings();
    std::unordered_map<std::string, std::string> endingTitles = {
        {"End1", "End 1: Ánh Sáng Thánh Hoàng (Holy Crown Order)"},
        {"End2", "End 2: Đế Chế Thiết Giáp (Iron Legion Dominion)"},
        {"End3", "End 3: Bóng Đêm Tội Phạm (Shadow Syndicate)"},
        {"End4", "End 4: Người Bảo Hộ Thiên Nhiên (Nature Wardens)"},
        {"End5", "End 5: [TRUE ENDING] Khôi Phục Vương Miện Vỡ (Fractured Crown Restored)"},
        {"GameOver", "Game Over: Hy Sinh Giữa Chiến Trường"}
    };

    if (allEndings.empty()) {
        std::cout << "    (Chưa có dữ liệu Endings)\n";
    } else {
        for (const auto& endId : allEndings) {
            bool unlocked = isNodeVisited(endId);
            std::string title = endId;
            if (endingTitles.find(endId) != endingTitles.end()) {
                title = endingTitles[endId];
            } else if (nodes.find(endId) != nodes.end() && !nodes.at(endId).title.empty()) {
                title = nodes.at(endId).title;
            }

            if (unlocked) {
                std::cout << "  [ " << ConsoleUI::colorize("★ ĐÃ MỞ KHÓA", ConsoleUI::Colors::BRIGHT_GREEN) << " ] " 
                          << ConsoleUI::colorize(title, ConsoleUI::Colors::BRIGHT_WHITE) << "\n";
            } else {
                std::cout << "  [ " << ConsoleUI::colorize("🔒 CHƯA MỞ", ConsoleUI::Colors::DIM) << " ] " 
                          << ConsoleUI::colorize(title, ConsoleUI::Colors::DIM) << "\n";
            }
        }
    }

    ConsoleUI::printDivider('=', 70, ConsoleUI::Colors::CYAN);
    std::cout << ConsoleUI::colorize("  ĐỊA ĐIỂM ĐÃ ĐI QUA GẦN ĐÂY:", ConsoleUI::Colors::BRIGHT_BLUE) << "\n";
    size_t showMax = visitedNodes.size() > 8 ? 8 : visitedNodes.size();
    for (size_t i = 0; i < showMax; ++i) {
        size_t idx = visitedNodes.size() - 1 - i;
        const std::string& vId = visitedNodes[idx];
        std::string nTitle = vId;
        if (nodes.find(vId) != nodes.end() && !nodes.at(vId).title.empty()) {
            nTitle = nodes.at(vId).title;
        }
        std::cout << "    -> [" << vId << "] " << nTitle << "\n";
    }
    if (visitedNodes.size() > 8) {
        std::cout << "    ... và " << (visitedNodes.size() - 8) << " địa điểm khác.\n";
    }

    std::cout << "\n";
    ConsoleUI::pause();
}
