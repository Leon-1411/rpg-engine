/**
 * @file play_story.cpp
 * @brief Chương trình chạy thử và tương tác trực tiếp với file data/story.json
 */

#include "StoryGraph.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

void printDivider() {
    std::cout << "\n============================================================\n";
}

int main() {
    // Thiết lập hiển thị UTF-8 trên Windows console nếu có thể
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    StoryGraph story;
    if (!story.loadStoryGraph("data/story.json")) {
        std::cerr << "Khong the nap data/story.json!\n";
        return 1;
    }

    std::vector<std::string> inventory;
    int exp = 0;

    std::cout << "\n============================================================\n";
    std::cout << "        CHƯƠNG TRÌNH CHẠY THỬ CỐT TRUYỆN RPG ENGINE         \n";
    std::cout << "============================================================\n";

    while (true) {
        StoryNode node = story.getCurrentNode();
        printDivider();
        std::cout << "Vị trí: " << (node.title.empty() ? node.id : node.title) << "\n";
        std::cout << "Mã node: [" << node.id << "] | Loại sự kiện: " << node.rawType << "\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << node.text << "\n\n";

        // Xử lý cơ chế Hội thoại rẽ nhánh (Branching Dialogue)
        if (story.isInDialogue()) {
            std::cout << "┌──────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 💬 HỘI THOẠI RẼ NHÁNH" 
                      << (!node.npcName.empty() ? (" - [" + node.npcName + "]") : "") 
                      << "\n";
            std::cout << "└──────────────────────────────────────────────────────────┘\n";

            while (story.isInDialogue()) {
                DialogueNode dNode = story.getCurrentDialogueNode();
                std::cout << "  🗣️  [" << dNode.speaker << "]:\n";
                std::cout << "     \"" << dNode.text << "\"\n\n";

                if (dNode.choices.empty()) {
                    break;
                }

                std::cout << "  👉 Đáp thoại của bạn:\n";
                for (size_t cIdx = 0; cIdx < dNode.choices.size(); ++cIdx) {
                    std::cout << "    [" << (cIdx + 1) << "] " << dNode.choices[cIdx].text << "\n";
                }

                int dChoice = 0;
                while (true) {
                    std::cout << "\n  👉 Chọn đáp thoại (1-" << dNode.choices.size() << "): ";
                    if (std::cin >> dChoice && dChoice >= 1 && dChoice <= static_cast<int>(dNode.choices.size())) {
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        break;
                    }
                    if (std::cin.eof()) return 0;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "  Lựa chọn không hợp lệ, vui lòng nhập lại!\n";
                }

                std::cout << "------------------------------------------------------------\n";
                std::string prevNodeId = story.getCurrentNode().id;
                story.selectDialogueChoice(dChoice - 1);

                // Nếu đáp thoại đã chuyển sang StoryNode khác
                if (story.getCurrentNode().id != prevNodeId) {
                    break;
                }
            }

            // Nếu việc chọn đáp thoại đã chuyển sang node tiếp theo, tiếp tục vòng lặp chính
            if (story.getCurrentNode().id != node.id) {
                continue;
            }
        }
        else if (!node.dialogues.empty()) {
            // Hiển thị đối thoại tuyến tính cho các node không dùng cây phân nhánh
            std::cout << "┌──────────────────────────────────────────────────────────┐\n";
            std::cout << "│ 💬 ĐỐI THOẠI" 
                      << (!node.npcName.empty() ? (" - [" + node.npcName + "]") : "") 
                      << "\n";
            std::cout << "└──────────────────────────────────────────────────────────┘\n";
            for (const auto& d : node.dialogues) {
                std::cout << "  🗣️  " << d.speaker << ":\n";
                std::cout << "     \"" << d.text << "\"\n\n";
            }
            std::cout << "------------------------------------------------------------\n";
        }

        // Xử lý các loại sự kiện theo data/story.json
        if (node.rawType == "STORY") {
            if (node.choices.empty()) {
                std::cout << "(Không còn lựa chọn tiếp theo)\n";
                break;
            }

            for (size_t i = 0; i < node.choices.size(); ++i) {
                std::cout << "  [" << (i + 1) << "] " << node.choices[i].text << "\n";
            }

            int choice = 0;
            while (true) {
                std::cout << "\n👉 Chọn hành động (1-" << node.choices.size() << "): ";
                if (std::cin >> choice && choice >= 1 && choice <= static_cast<int>(node.choices.size())) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
                if (std::cin.eof()) return 0;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Lựa chọn không hợp lệ, vui lòng nhập lại!\n";
            }

            story.selectChoice(choice - 1);
        }
        else if (node.rawType == "COMBAT") {
            std::cout << "⚔️  ĐỐI THỦ: " << node.enemyId << "\n";
            std::cout << "  [1] Quyết chiến và đánh bại quái vật (Thắng -> " << node.onWinNodeId << ")\n";
            std::cout << "  [2] Thất bại trước sức mạnh quái thú (Thua -> " << node.onLoseNodeId << ")\n";

            int action = 0;
            while (true) {
                std::cout << "\n👉 Chọn kết quả trận đấu thử nghiệm (1: Thắng, 2: Thua): ";
                if (std::cin >> action && (action == 1 || action == 2)) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
                if (std::cin.eof()) return 0;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            if (action == 1) {
                std::cout << "🎉 Bạn đã giành chiến thắng vang dội!\n";
                story.moveToNode(node.onWinNodeId);
            } else {
                std::cout << "💀 Bạn đã gục ngã trong trận chiến...\n";
                story.moveToNode(node.onLoseNodeId);
            }
        }
        else if (node.rawType == "REWARD") {
            std::cout << "🎁 PHẦN THƯỞNG NHẬN ĐƯỢC:\n";
            if (node.rewardExp > 0) {
                exp += node.rewardExp;
                std::cout << "  + Kinh nghiệm (EXP): +" << node.rewardExp << " (Tổng: " << exp << ")\n";
            }
            for (const auto& item : node.rewardItems) {
                inventory.push_back(item);
                std::cout << "  + Vật phẩm: [" << item << "]\n";
            }

            std::cout << "\nNhấn Enter để tiếp tục hành trình...";
            if (!std::cin.eof()) {
                std::string line;
                std::getline(std::cin, line);
            }

            story.moveToNode(node.nextNodeId);
        }
        else if (node.rawType == "REQUIREMENT_CHECK") {
            std::cout << "🔒 KIỂM TRA ĐIỀU KIỆN:\n";
            std::cout << "Yêu cầu vật phẩm: [" << node.requiredItem << "]\n";

            bool hasItem = std::find(inventory.begin(), inventory.end(), node.requiredItem) != inventory.end();
            if (hasItem) {
                std::cout << "✅ Bạn ĐÃ CÓ [" << node.requiredItem << "] trong túi đồ! Cánh cửa mở ra...\n";
                std::cout << "\nNhấn Enter để đi tiếp...";
                if (!std::cin.eof()) {
                    std::string line;
                    std::getline(std::cin, line);
                }
                story.moveToNode(node.onPassNodeId);
            } else {
                std::cout << "❌ Bạn CHƯA CÓ [" << node.requiredItem << "]! Không thể mở cửa phong ấn.\n";
                std::cout << "\nNhấn Enter để đi tiếp...";
                if (!std::cin.eof()) {
                    std::string line;
                    std::getline(std::cin, line);
                }
                story.moveToNode(node.onFailNodeId);
            }
        }
        else if (node.rawType == "GAME_OVER" || node.rawType == "VICTORY" || story.isEnding()) {
            std::cout << "************************************************************\n";
            if (node.rawType == "VICTORY") {
                std::cout << "🏆 CHÚC MỪNG BẠN ĐÃ CHIẾN THẮNG TRÒ CHƠI!\n";
            } else {
                std::cout << "☠️  TRÒ CHƠI KẾT THÚC (GAME OVER)!\n";
            }
            std::cout << "************************************************************\n\n";

            std::cout << "  [1] Chơi lại từ đầu (Reset)\n";
            std::cout << "  [2] Thoát\n";
            std::cout << "\n👉 Lựa chọn: ";
            int endChoice = 0;
            if (!(std::cin >> endChoice) || std::cin.eof()) {
                break;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (endChoice == 1) {
                inventory.clear();
                exp = 0;
                story.loadStoryGraph("data/story.json");
                continue;
            } else {
                std::cout << "Cảm ơn bạn đã trải nghiệm thử cốt truyện!\n";
                break;
            }
        }
    }

    return 0;
}
