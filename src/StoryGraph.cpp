/**
 * @file StoryGraph.cpp
 * @brief Implement StoryGraph class methods.
 * @author Nghĩa
 */

#include "StoryGraph.h"

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
    return true;
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
