#include "StoryGraph.h"
#include <cassert>
#include <iostream>

int main() {
    StoryGraph story;
    StoryNode node = story.getCurrentNode();
    assert(node.id == "start");
    assert(node.choices.size() == 2);

    story.addNode({"forest_path", "Deep inside the forest...", EventType::NORMAL, {}});
    assert(story.selectChoice(0) == true);
    assert(story.getCurrentNode().id == "forest_path");

    std::cout << "[PASS] StoryGraph unit tests successful!\n";
    return 0;
}
