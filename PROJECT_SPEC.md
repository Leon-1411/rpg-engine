# RPG Engine --- Project Specification

## 1. Tổng quan

**RPG Engine** là game nhập vai C++17 chạy trên Console, tập trung vào
lập trình hướng đối tượng, chiến đấu theo lượt, quản lý nhân vật/vật
phẩm, cốt truyện phân nhánh và Save/Load.

### Công nghệ

-   C++17
-   CMake
-   Feature-Based Architecture
-   `nlohmann/json`
-   Console UI + ASCII Art
-   Git/GitHub
-   JSON cho dữ liệu game và save

### Gameplay Loop

``` mermaid
flowchart TD
    A[Main Menu] --> B{New Game / Load Game}
    B -->|New Game| C[Choose Hero]
    B -->|Load Game| D[Load GameState]
    C --> E[Story]
    D --> E
    E --> F[Player Choice]
    F --> G{Event Type}
    G -->|Battle| H[Combat]
    G -->|Item| I[Receive Item]
    G -->|Shop| J[Shop]
    G -->|Normal| E
    H --> K{Win?}
    K -->|Yes| L[EXP / Reward]
    K -->|No| M[Game Over]
    L --> N[Level Up if eligible]
    N --> E
    I --> E
    J --> E
    E --> O[Boss]
    O --> P[Ending]
```

------------------------------------------------------------------------

## 2. Phạm vi chức năng

Phiên bản hoàn chỉnh cần có:

-   3 Hero: Warrior, Mage, Ranger
-   Skill riêng cho từng Hero
-   Minion và BossMonster
-   Enemy AI / Boss attack pattern
-   Turn-Based Combat
-   Damage, Defense, Cooldown, Status Effect
-   Inventory
-   Weapon, Armor, Potion
-   EXP, Level, Stat Progression
-   Branching Narrative
-   StoryNode và Choice
-   Ít nhất 2 Ending
-   Save/Load nhiều slot
-   Console UI
-   ASCII Art
-   Dữ liệu game bằng JSON
-   Input validation

------------------------------------------------------------------------

## 3. Kiến trúc tổng thể

Sử dụng **Feature-Based Architecture**. Mỗi hệ thống của game nằm trong
module riêng.

``` mermaid
flowchart TD
    Main[main.cpp] --> Game[Game]
    Game --> Story[StoryManager]
    Game --> Combat[CombatSystem]
    Game --> Save[SaveManager]
    Game --> UI[ConsoleUI]
    Game --> Progression[LevelSystem]

    Story --> StoryNode[StoryNode / Choice]
    Combat --> Hero[Hero]
    Combat --> Enemy[Enemy]
    Hero --> Inventory[Inventory]
    Inventory --> Item[Item]
    Save --> State[GameState]

    UI -. hiển thị .-> Hero
    UI -. hiển thị .-> Enemy
    UI -. hiển thị .-> Inventory
```

### Nguyên tắc

-   `Game` là coordinator chính.
-   UI chỉ hiển thị và nhận input, không tính damage hoặc xử lý business
    logic.
-   `CombatSystem` không phụ thuộc trực tiếp vào Console.
-   Dữ liệu Enemy, Item, Skill, Story không hard-code nếu có thể đưa vào
    JSON.
-   Tránh circular dependency.
-   Ưu tiên interface/base class và forward declaration khi phù hợp.

------------------------------------------------------------------------

## 4. Folder Structure

``` text
RPGEngine/
├── CMakeLists.txt
├── README.md
├── PROJECT_SPEC.md
├── main.cpp
│
├── include/
│   ├── hero/
│   │   ├── Hero.h
│   │   ├── Warrior.h
│   │   ├── Mage.h
│   │   ├── Ranger.h
│   │   └── Skill.h
│   │
│   ├── enemy/
│   │   ├── Enemy.h
│   │   ├── Minion.h
│   │   └── BossMonster.h
│   │
│   ├── combat/
│   │   ├── CombatSystem.h
│   │   ├── DamageCalculator.h
│   │   └── StatusEffect.h
│   │
│   ├── item/
│   │   ├── Item.h
│   │   ├── Weapon.h
│   │   ├── Armor.h
│   │   ├── Potion.h
│   │   └── Inventory.h
│   │
│   ├── story/
│   │   ├── Choice.h
│   │   ├── StoryNode.h
│   │   └── StoryManager.h
│   │
│   ├── progression/
│   │   └── LevelSystem.h
│   │
│   ├── save/
│   │   ├── GameState.h
│   │   └── SaveManager.h
│   │
│   ├── ui/
│   │   ├── ConsoleUI.h
│   │   ├── MainMenu.h
│   │   ├── BattleUI.h
│   │   ├── InventoryUI.h
│   │   └── ASCIIArt.h
│   │
│   └── game/
│       └── Game.h
│
├── src/
│   ├── hero/
│   │   ├── Hero.cpp
│   │   ├── Warrior.cpp
│   │   ├── Mage.cpp
│   │   ├── Ranger.cpp
│   │   └── Skill.cpp
│   ├── enemy/
│   │   ├── Enemy.cpp
│   │   ├── Minion.cpp
│   │   └── BossMonster.cpp
│   ├── combat/
│   │   ├── CombatSystem.cpp
│   │   ├── DamageCalculator.cpp
│   │   └── StatusEffect.cpp
│   ├── item/
│   │   ├── Item.cpp
│   │   ├── Weapon.cpp
│   │   ├── Armor.cpp
│   │   ├── Potion.cpp
│   │   └── Inventory.cpp
│   ├── story/
│   │   ├── Choice.cpp
│   │   ├── StoryNode.cpp
│   │   └── StoryManager.cpp
│   ├── progression/
│   │   └── LevelSystem.cpp
│   ├── save/
│   │   ├── GameState.cpp
│   │   └── SaveManager.cpp
│   ├── ui/
│   │   ├── ConsoleUI.cpp
│   │   ├── MainMenu.cpp
│   │   ├── BattleUI.cpp
│   │   ├── InventoryUI.cpp
│   │   └── ASCIIArt.cpp
│   └── game/
│       └── Game.cpp
│
├── data/
│   ├── story.json
│   ├── enemies.json
│   ├── items.json
│   └── skills.json
│
├── saves/
│   ├── slot1.json
│   ├── slot2.json
│   └── slot3.json
│
└── tests/
    ├── HeroTest.cpp
    ├── EnemyTest.cpp
    ├── CombatTest.cpp
    ├── InventoryTest.cpp
    ├── StoryTest.cpp
    └── SaveLoadTest.cpp
```

------------------------------------------------------------------------

## 5. Hero System

### Class hierarchy

``` mermaid
classDiagram
    class Hero {
        <<abstract>>
        -string name
        -int level
        -int exp
        -int hp
        -int maxHp
        -int mp
        -int maxMp
        -int attack
        -int defense
        +normalAttack()
        +useSkill()*
        +takeDamage()
        +isAlive()
    }

    Hero <|-- Warrior
    Hero <|-- Mage
    Hero <|-- Ranger

    Hero o-- Inventory
    Hero o-- Skill
```

### Warrior

-   HP/DEF cao
-   Power Slash
-   Shield Block
-   Berserk

### Mage

-   MP và skill damage cao
-   Fireball
-   Ice Blast
-   Meteor

### Ranger

-   Crit/dodge tốt
-   Double Shot
-   Poison Arrow
-   Rain of Arrows

`Hero` là abstract base class. Behavior đặc trưng của từng class được
triển khai bằng polymorphism.

------------------------------------------------------------------------

## 6. Enemy System

``` mermaid
classDiagram
    class Enemy {
        <<abstract>>
        -string name
        -int hp
        -int attack
        -int defense
        -int expReward
        -int goldReward
        +chooseAction()
        +takeDamage()
        +isAlive()
    }

    Enemy <|-- Minion
    Enemy <|-- BossMonster
```

Minion có thể gồm Goblin, Skeleton, Orc, Dark Knight.

Boss có attack pattern riêng, ví dụ:

1.  Normal Attack
2.  Heavy Attack
3.  Special Skill
4.  Quay lại pattern hoặc chuyển phase.

------------------------------------------------------------------------

## 7. Combat System

`CombatSystem` chịu trách nhiệm điều khiển trận đấu:

-   `startBattle()`
-   `playerTurn()`
-   `enemyTurn()`
-   `useSkill()`
-   `applyStatusEffect()`
-   `updateCooldowns()`
-   `checkBattleResult()`

`DamageCalculator` chịu trách nhiệm công thức damage.

`StatusEffect` đại diện các trạng thái như poison, buff defense, giảm
attack.

### Player actions

1.  Attack
2.  Skill
3.  Item
4.  Defend
5.  Run (chỉ khi trận đấu cho phép)

`CombatSystem` không tự in menu. `BattleUI` nhận trạng thái combat và
hiển thị.

------------------------------------------------------------------------

## 8. Item & Inventory

``` mermaid
classDiagram
    class Item {
        <<abstract>>
        -string id
        -string name
        -string description
    }

    Item <|-- Weapon
    Item <|-- Armor
    Item <|-- Potion
    Inventory o-- Item
```

### Inventory

-   Add item
-   Remove item
-   Use item
-   Equip Weapon
-   Equip Armor
-   Kiểm tra số lượng
-   Truy xuất item cho UI

Ví dụ item:

-   Iron Sword
-   Magic Staff
-   Long Bow
-   Iron Armor
-   Magic Robe
-   Leather Armor
-   Health Potion
-   Mana Potion

------------------------------------------------------------------------

## 9. Progression

`LevelSystem` xử lý:

-   EXP requirement
-   Add EXP
-   Level Up
-   Stat points
-   Stat progression

Khi lên level, người chơi có thể tăng HP, MP, ATK hoặc DEF tùy thiết kế
gameplay.

------------------------------------------------------------------------

## 10. Story System

``` mermaid
classDiagram
    class StoryNode {
        +string id
        +string text
        +EventType type
        +vector~Choice~ choices
    }

    class Choice {
        +string text
        +string nextNodeId
        +string requiredFlag
        +string setFlag
    }

    class StoryManager {
        -string currentNodeId
        +getCurrentNode()
        +selectChoice()
        +moveToNode()
    }

    StoryManager --> StoryNode
    StoryNode o-- Choice
```

### Event Type

-   NORMAL
-   BATTLE
-   ITEM
-   SHOP
-   BOSS
-   ENDING

Choice có thể thay đổi story flags để mở/khóa các nhánh khác nhau.

Tối thiểu có: - Good Ending - Bad Ending

------------------------------------------------------------------------

## 11. Data-Driven JSON

### `enemies.json`

``` json
{
  "goblin": {
    "name": "Goblin",
    "hp": 60,
    "attack": 12,
    "defense": 5,
    "expReward": 40,
    "goldReward": 10
  }
}
```

### `items.json`

Chứa ID, tên, loại, description và stat/effect của item.

### `skills.json`

Chứa skill ID, damage multiplier, MP cost, cooldown và effect.

### `story.json`

Chứa StoryNode, text, event type, choices, next node và các story flags.

Mục tiêu của data-driven design là cho phép cân bằng/thêm nội dung mà
không phải sửa trực tiếp logic C++.

------------------------------------------------------------------------

## 12. Save / Load

`GameState` là snapshot trạng thái hiện tại của game.

Save tối thiểu:

-   Hero type/name
-   Level và EXP
-   HP/MP
-   Stats
-   Gold
-   Inventory
-   Equipment
-   Current StoryNode
-   Story flags

`SaveManager`:

-   Save slot
-   Load slot
-   Delete slot
-   Validate save data

Không serialize pointer hoặc object graph trực tiếp. Chỉ lưu dữ liệu cần
thiết và tái tạo object khi load.

------------------------------------------------------------------------

## 13. Console UI

UI gồm:

-   `ConsoleUI`: tiện ích chung, input validation
-   `MainMenu`: New Game / Load / Exit
-   `BattleUI`: battle screen, HP/MP bars, action menu
-   `InventoryUI`: inventory/equipment menu
-   `ASCIIArt`: Hero/Enemy/Boss art

Ví dụ:

``` text
====================================
             BATTLE
====================================

Mage                     Goblin

HP: 80/80                 HP: 60/60
MP: 100/120

1. Attack
2. Skill
3. Item
4. Defend
5. Run

Choose:
```

UI không được trực tiếp thay đổi HP, EXP, inventory hoặc story state. UI
chỉ gửi lựa chọn của người chơi về các system tương ứng.

------------------------------------------------------------------------

## 14. Dependency Rules

Cho phép:

``` text
Game -> Story / Combat / Save / UI / Progression
Combat -> Hero / Enemy / Item
Hero -> Skill / Inventory
Inventory -> Item
StoryManager -> StoryNode / Choice
SaveManager -> GameState
UI -> read-only game state / public interfaces
```

Hạn chế:

``` text
Hero X-> ConsoleUI
Enemy X-> ConsoleUI
Item X-> ConsoleUI
CombatSystem X-> MainMenu
StoryNode X-> Game
```

Các base class không nên phụ thuộc vào module cấp cao hơn.

------------------------------------------------------------------------

## 15. Phân công 7 thành viên

  -----------------------------------------------------------------------
  Thành viên              Module                  Trách nhiệm
  ----------------------- ----------------------- -----------------------
  1 --- Leader            `game`, `save`          Architecture, Game
                                                  loop, Save/Load,
                                                  integration

  2                       `hero`, `progression`   Hero classes, Skills,
                                                  EXP, Level

  3                       `enemy`                 Enemy hierarchy,
                                                  Minion, Boss, AI

  4                       `combat`                Turn system, damage,
                                                  cooldown, status

  5                       `item`                  Item hierarchy,
                                                  equipment, inventory

  6                       `story`                 StoryNode, Choice,
                                                  branching story, story
                                                  JSON

  7                       `ui`, `tests`           Console UI, ASCII Art,
                                                  input validation, QA
  -----------------------------------------------------------------------

### Contract phải chốt trước khi code

Team phải thống nhất sớm:

-   Public interface của `Hero`
-   Public interface của `Enemy`
-   `CombatResult`
-   `Item`/Inventory API
-   StoryNode schema
-   GameState schema
-   JSON ID convention
-   EventType
-   Error/input handling

Không tự ý sửa public interface của module khác mà không thông báo team.

------------------------------------------------------------------------

## 16. Git Workflow

Branches chính:

``` text
main
develop
feature/hero
feature/enemy
feature/combat
feature/item
feature/story
feature/save
feature/ui
```

Flow:

``` text
feature/*
   ↓
Pull Request
   ↓
develop
   ↓
Integration Test
   ↓
main
```

Không push trực tiếp lên `main`.

Commit nên nhỏ, rõ nghĩa:

``` text
feat(hero): add Mage skill system
fix(combat): prevent negative damage
test(save): add save-load validation
refactor(item): separate equipment logic
```

------------------------------------------------------------------------

## 17. Coding Convention

-   Class: `PascalCase`
-   Function/method: `camelCase`
-   Variable: `camelCase`
-   Constant: `UPPER_SNAKE_CASE`
-   Mỗi class chính nên có `.h` và `.cpp`.
-   Dùng `#pragma once` hoặc include guards thống nhất.
-   Ưu tiên `std::unique_ptr`/`std::shared_ptr` khi cần ownership động
    thay vì raw owning pointer.
-   Dùng `const` khi object không bị thay đổi.
-   Tránh global mutable state.
-   Không dùng `using namespace std;` trong header.
-   Public interface nhỏ và rõ ràng.
-   Một function nên có một trách nhiệm chính.

------------------------------------------------------------------------

## 18. Testing Strategy

Các test quan trọng:

### Hero

-   Khởi tạo đúng stats.
-   Take damage.
-   Hero chết khi HP \<= 0.
-   Skill tiêu MP đúng.

### Enemy

-   Stats đúng.
-   AI trả action hợp lệ.
-   Boss pattern chạy đúng.

### Combat

-   Damage không âm.
-   Defense ảnh hưởng damage.
-   Cooldown cập nhật đúng.
-   Status effect hết đúng turn.
-   Battle kết thúc khi một bên chết.

### Inventory

-   Add/remove.
-   Stack potion.
-   Equip/unequip.
-   Không dùng item không tồn tại.

### Level

-   EXP tăng đúng.
-   Level up đúng threshold.
-   Stats tăng đúng.

### Story

-   Choice chuyển đúng node.
-   Flag mở đúng branch.
-   Ending đúng điều kiện.

### Save/Load

-   Save rồi load cho GameState tương đương.
-   Slot không tồn tại.
-   JSON lỗi/corrupt không làm game crash.

### UI

-   Nhập chữ khi yêu cầu số.
-   Nhập ngoài range.
-   EOF/input invalid.

------------------------------------------------------------------------

## 19. Roadmap

### Phase 1 --- Architecture

-   Folder structure
-   CMake
-   Base interfaces
-   Shared enums/types
-   JSON schema

### Phase 2 --- Core Entities

Làm song song: - Hero - Enemy - Item - StoryNode

### Phase 3 --- Combat

Ghép Hero + Enemy + Skill + Item.

### Phase 4 --- Progression & Inventory

Hoàn thiện Level, equipment và reward.

### Phase 5 --- Story

Ghép StoryManager với event/battle.

### Phase 6 --- Save/Load

Tạo GameState và serialization.

### Phase 7 --- Console UI

Menu, BattleUI, InventoryUI, ASCII Art.

### Phase 8 --- Integration

Game điều phối tất cả module.

### Phase 9 --- Testing & Balancing

Test, sửa bug, cân bằng stats.

### Phase 10 --- Final Demo

Hoàn thiện story, boss, endings và demo flow.

------------------------------------------------------------------------

## 20. Definition of Done

Project chỉ được xem là hoàn thành khi:

-   [ ] Build thành công bằng CMake.
-   [ ] Có Warrior, Mage, Ranger.
-   [ ] Mỗi Hero có skill riêng.
-   [ ] Có ít nhất 3 loại Minion.
-   [ ] Có ít nhất 1 Boss với AI/pattern.
-   [ ] Combat turn-based hoạt động.
-   [ ] Có damage, defense, skill, cooldown.
-   [ ] Có ít nhất một Status Effect.
-   [ ] Inventory hoạt động.
-   [ ] Weapon, Armor, Potion hoạt động.
-   [ ] Có EXP và Level Up.
-   [ ] Có StoryNode + Choice.
-   [ ] Có branching story.
-   [ ] Có ít nhất 2 endings.
-   [ ] Save/Load hoạt động.
-   [ ] Có nhiều save slot.
-   [ ] Console UI có input validation.
-   [ ] Hero/Enemy/Boss có thể hiển thị ASCII Art.
-   [ ] Enemy/Item/Skill/Story được đọc từ JSON ở các phần phù hợp.
-   [ ] Không có circular dependency nghiêm trọng.
-   [ ] Các module chính có test.
-   [ ] Game có thể chơi từ New Game đến Ending mà không crash.

------------------------------------------------------------------------

## 21. Implementation Order for AI Agent

Khi dùng AI coding agent, triển khai theo đúng thứ tự sau và không tự ý
nhảy phase:

1.  `CMakeLists.txt` và folder structure.
2.  Shared enums/types cần thiết.
3.  `Item` → `Weapon` → `Armor` → `Potion`.
4.  `Inventory`.
5.  `Skill`.
6.  `Hero`.
7.  `Warrior`, `Mage`, `Ranger`.
8.  `Enemy`.
9.  `Minion`, `BossMonster`.
10. `DamageCalculator`.
11. `StatusEffect`.
12. `CombatSystem`.
13. `LevelSystem`.
14. `Choice`.
15. `StoryNode`.
16. `StoryManager`.
17. JSON loaders và `data/*.json`.
18. `GameState`.
19. `SaveManager`.
20. `ConsoleUI`.
21. `MainMenu`.
22. `BattleUI`.
23. `InventoryUI`.
24. `ASCIIArt`.
25. `Game`.
26. `main.cpp`.
27. Unit tests.
28. Integration tests.
29. Game balancing.
30. Final story/boss/endings.

### Quy tắc cho AI Agent

Trước khi implement một phase:

1.  Đọc `PROJECT_SPEC.md`.
2.  Kiểm tra public interfaces hiện có.
3.  Không sửa interface của module khác nếu không thật sự cần.
4.  Chỉ implement phase được yêu cầu.
5.  Build sau mỗi module.
6.  Chạy test liên quan.
7.  Không tự ý đổi architecture.
8.  Không đưa UI logic vào domain/game logic.
9.  Không hard-code dữ liệu có thể nằm trong JSON.
10. Báo rõ file đã tạo/sửa sau mỗi phase.

------------------------------------------------------------------------

## 22. Mục tiêu cuối cùng

Kiến trúc phải cho phép luồng:

``` text
Start Game
    ↓
Choose Hero
    ↓
Explore Story
    ↓
Make Choices
    ↓
Battle / Event / Item
    ↓
Gain EXP & Equipment
    ↓
Story Branches
    ↓
Boss Battle
    ↓
Good / Bad Ending
```

Project ưu tiên **code rõ ràng, OOP đúng, module độc lập, dễ chia việc
cho 7 thành viên và dễ mở rộng** hơn việc thêm quá nhiều tính năng.
