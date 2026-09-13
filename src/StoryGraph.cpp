 /**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods.
 * @author Nghĩa
 */

#include "StoryGraph.h"
#include <fstream>
#include <nlohmann/json.hpp>

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

bool StoryGraph::loadStoryGraph(const std::string& filePath) {
    std::cout << "[StoryGraph] Loading story data from " << filePath << "...\n";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[StoryGraph] Error: Could not open file " << filePath << "\n";
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;
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
                for (auto& [key, nodeJson] : j["nodes"].items()) {
                    StoryNode node;
                    node.id = key;
                    if (nodeJson.contains("id")) node.id = nodeJson["id"].get<std::string>();
                    if (nodeJson.contains("title")) node.title = nodeJson["title"].get<std::string>();
                    if (nodeJson.contains("description")) node.text = nodeJson["description"].get<std::string>();
                    else if (nodeJson.contains("text")) node.text = nodeJson["text"].get<std::string>();

                    std::string typeStr = nodeJson.value("type", "STORY");
                    node.rawType = typeStr;
                    if (typeStr == "COMBAT" || typeStr == "BATTLE") node.type = EventType::BATTLE;
                    else if (typeStr == "REWARD" || typeStr == "ITEM") node.type = EventType::ITEM;
                    else if (typeStr == "SHOP") node.type = EventType::SHOP;
                    else if (typeStr == "GAME_OVER" || typeStr == "VICTORY" || typeStr == "ENDING") node.type = EventType::ENDING;
                    else node.type = EventType::NORMAL;

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
                            for (auto& [dKey, dVal] : dtJson["nodes"].items()) {
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
                    if (typeStr == "BATTLE" || typeStr == "COMBAT") node.type = EventType::BATTLE;
                    else if (typeStr == "ITEM" || typeStr == "REWARD") node.type = EventType::ITEM;
                    else if (typeStr == "SHOP") node.type = EventType::SHOP;
                    else if (typeStr == "ENDING" || typeStr == "GAME_OVER" || typeStr == "VICTORY") node.type = EventType::ENDING;
                    else node.type = EventType::NORMAL;

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
                            for (auto& [dKey, dVal] : dtJson["nodes"].items()) {
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

        resetDialogue();
        std::cout << "[StoryGraph] Successfully loaded " << nodes.size() << " nodes from " << filePath << ".\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[StoryGraph] Parse exception: " << e.what() << "\n";
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

bool StoryGraph::selectDialogueChoice(int choiceIndex) {
    if (!isInDialogue()) return false;
    DialogueNode dNode = getCurrentDialogueNode();
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(dNode.choices.size())) {
        return false;
    }

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

bool StoryGraph::isEnding() const {
    return getCurrentNode().type == EventType::ENDING;
}

size_t StoryGraph::getNodeCount() const {
    return nodes.size();
}

const std::unordered_map<std::string, StoryNode>& StoryGraph::getAllNodes() const {
    return nodes;
}

