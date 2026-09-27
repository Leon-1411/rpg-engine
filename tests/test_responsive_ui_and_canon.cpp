#include "ui/ConsoleUI.h"
#include "DataLoader.h"
#include "Hero.h"
#include "Warrior.h"
#include "Minion.h"
#include "BossMonster.h"
#include "Potion.h"
#include <cassert>
#include <iostream>
#include <vector>

void testResponsiveWordWrap() {
    std::cout << "Testing ConsoleUI UTF-8 word wrapping & display width...\n";

    std::string viText = "Tại đại điện Eldoria nguy nga, Nhà Vua trao mật lệnh: Công chúa Elena đã bị Ma Vương bắt cóc trong đêm Trăng Máu.";
    
    // Wrap at 40 chars
    auto lines40 = ConsoleUI::wrapText(viText, 40);
    assert(!lines40.empty());
    for (const auto& l : lines40) {
        size_t w = ConsoleUI::getDisplayWidth(l);
        assert(w <= 40);
    }

    // Wrap at 60 chars
    auto lines60 = ConsoleUI::wrapText(viText, 60);
    assert(!lines60.empty());
    for (const auto& l : lines60) {
        size_t w = ConsoleUI::getDisplayWidth(l);
        assert(w <= 60);
    }

    // Terminal width clamped within [60, 110]
    int termWidth = ConsoleUI::getTerminalWidth();
    assert(termWidth >= 60 && termWidth <= 110);

    std::cout << "[PASS] UTF-8 Word Wrap & Terminal Width tests passed!\n";
}

void testCanonEnemyClassification() {
    std::cout << "Testing 3 Canon Minions & 6 Canon Bosses via DataLoader::loadEnemyById...\n";

    // 1. Test 3 Minions
    std::vector<std::string> canonMinions = {
        "Wild_Mercenary",
        "Demon_Scout",
        "Demon_Berserker"
    };

    for (const auto& mId : canonMinions) {
        auto enemy = DataLoader::loadEnemyById("data/enemies.json", mId);
        assert(enemy != nullptr);
        assert(enemy->getType() == EnemyType::MINION);
        assert(enemy->isAlive());
        assert(enemy->getHp() > 0);
    }

    // 2. Test 6 Bosses
    std::vector<std::string> canonBosses = {
        "Demon_King_Malakor",
        "General_Aldric",
        "The_Core_Guardian",
        "Arcane_Council_Enforcers",
        "Archmage_Morvath",
        "Multi-Faction Battle"
    };

    for (const auto& bId : canonBosses) {
        auto boss = DataLoader::loadEnemyById("data/enemies.json", bId);
        assert(boss != nullptr);
        assert(boss->getType() == EnemyType::BOSS);
        assert(boss->isAlive());
        assert(boss->getHp() >= 400);
    }

    // 3. Verify Wild_Mercenary is NEVER loaded as BOSS by BossFactory
    auto invalidBoss = BossFactory::createFromJson("Wild_Mercenary", "data/enemies.json");
    assert(invalidBoss == nullptr);

    // 4. Verify Demon_King_Malakor is NEVER loaded as MINION by MinionFactory
    auto invalidMinion = MinionFactory::createFromJson("Demon_King_Malakor", "data/enemies.json");
    assert(invalidMinion == nullptr);

    std::cout << "[PASS] Canon Enemy classification tests passed!\n";
}

void testFreeActionPotionLogic() {
    std::cout << "Testing Free Action Potion healing and full-stat validation...\n";

    Warrior hero("Arthur", 100, 30, 20, 10);
    hero.getInventory().clear();

    auto hpPot = std::make_shared<Potion>("pot_hp", "Bình Máu", "Hồi 50 HP", 50, false, 2);
    auto mpPot = std::make_shared<Potion>("pot_mp", "Bình Mana", "Hồi 30 MP", 30, true, 2);
    hero.getInventory().addItem(hpPot);
    hero.getInventory().addItem(mpPot);

    // Initial: HP is 100/100 (Full) -> Should not consume
    assert(hero.getHp() == hero.getMaxHp());
    // Simulate damage
    hero.takeDirectDamage(60);
    assert(hero.getHp() == 40);

    // Use HP Potion
    int beforeHp = hero.getHp();
    assert(hero.getInventory().useItem(0, hero) == true);
    assert(hero.getHp() == beforeHp + 50); // 40 + 50 = 90
    assert(hero.getInventory().getItemCount() == 2); // Still has 1 pot left (quantity 1)

    std::cout << "[PASS] Free Action Potion logic tests passed!\n";
}

int main() {
    std::cout << "===================================================\n";
    std::cout << "  RUNNING RESPONSIVE UI & CANON ENEMY TEST SUITE   \n";
    std::cout << "===================================================\n";

    testResponsiveWordWrap();
    testCanonEnemyClassification();
    testFreeActionPotionLogic();

    std::cout << "===================================================\n";
    std::cout << "  [ALL PASS] Responsive UI & Canon tests passed!   \n";
    std::cout << "===================================================\n";
    return 0;
}
