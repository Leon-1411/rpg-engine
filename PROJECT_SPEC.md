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

## 5. Hero System

### Class hierarchy

``` mermaid
classDiagram
    class Hero {
        <<abstract>>
        -string name
        -HeroClass heroClass
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
        +getHeroClassName() string
    }

    Hero <|-- Warrior
    Hero <|-- Mage
    Hero <|-- Ranger

    Hero o-- Inventory
    Hero o-- Skill
```

### 1. Warrior (Arthur)
- **Thuộc tính:** HP và DEF cực cao, phong cách cận chiến vững chắc.
- **Kỹ năng:** Power Slash, Shield Block, Berserk.
- **Lợi thế cốt truyện:** Khắc chế các Boss vật lý (`General_Aldric`, `Demon_Berserker`). Mở ra các nhánh hội thoại đặc biệt liên quan đến Lời thề Hiệp sĩ và Danh dự Vương quốc.

### 2. Mage (Morrigan)
- **Thuộc tính:** MP dồi dào, sát thương phép diện rộng và khắc chế ma thuật.
- **Kỹ năng:** Fireball, Ice Blast, Meteor.
- **Lợi thế cốt truyện:** Dễ dàng giải mã các phong ấn cổ ngữ tại Tháp Viện Ma Pháp (Node 04 -> Node 07), tự động tìm thấy `AncientCodex`; giảm sát thương ma thuật từ `Archmage_Morvath`.

### 3. Ranger (Lyra)
- **Thuộc tính:** Tỷ lệ Crit/Dodge cao, tốc độ đi trước trong lượt đấu.
- **Kỹ năng:** Double Shot, Poison Arrow, Rain of Arrows.
- **Lợi thế cốt truyện:** Phát hiện dấu vết bí mật qua Rừng Sâu (Whispering Forest), né tránh bẫy phục kích và tìm ra các rương cổ vật ẩn chứa bình thuốc quý.

------------------------------------------------------------------------

## 6. Enemy System

``` mermaid
classDiagram
    class Enemy {
        <<abstract>>
        -string name
        -EnemyType type
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

### Danh sách Kẻ địch & Bosses theo Cốt truyện (Fractured Crown)

#### Minions:
1. **`Wild_Mercenary`** (Bộ Tộc Tự Do - Node 03): Lính đánh thuê và thợ săn du mục canh giữ ranh giới Rừng Rậm Huyết Nguyệt.
2. **`Demon_Scout`** (Ma Tộc - Node 05): Trinh sát quỷ nhanh nhẹn tuần tra vùng biên giới Trăng Máu.
3. **`Demon_Berserker`** (Ma Tộc - Node 12): Chiến binh quỷ cuồng nộ canh giữ lối vào Ma Điện khi người chơi đột kích trực diện.

#### 6 Faction Bosses:
1. **`Demon_King_Malakor`** (Ma Tộc - Node 14): Thống lĩnh Ma tộc mang sức mạnh hỏa ngục và bóng tối (Nhánh 1 - Vương Quốc).
2. **`General_Aldric`** (Hoàng Gia - Node 17): Đại tướng huyền thoại của Eldoria bảo vệ buồng The Core (Nhánh 2 - Elena & Ma Tộc).
3. **`The_Core_Guardian`** (Vệ Thần Cổ Đại - Node 18): Cỗ máy hộ vệ khổng lồ bảo vệ lõi The Core (Nhánh 2 - Elena & Ma Tộc).
4. **`Arcane_Council_Enforcers`** (Hội Đồng Pháp Sư - Node 19): Đội cấm vệ pháp sư bảo vệ trận địa cấm thuật (Nhánh 3 - Pháp Sư).
5. **`Archmage_Morvath`** (Hội Đồng Pháp Sư - Node 20): Trưởng lão tối cao mưu mô của Hội đồng Pháp sư (Nhánh 3 - Pháp Sư).
6. **`Multi-Faction Battle`** (Hỗn Hợp - Node 22): Trận huyết chiến hỗn loạn giữa tàn quân Hoàng Gia, Ma tộc và Pháp sư (Nhánh 4 - Bộ Tộc Tự Do).

------------------------------------------------------------------------

## 7. Combat System

`CombatEngine` / `CombatSystem` điều khiển trận đấu theo lượt:
- `startBattle()`
- `executeTurn(actionCode)`
- `playerTurn()`
- `enemyTurn()`
- `useSkill()`
- `applyStatusEffect()`
- `checkBattleResult()`

### Player actions
1. Attack (Đánh thường)
2. Skill (Dùng kỹ năng tốn MP)
3. Item (Dùng Potion trong Inventory)
4. Defend (Thủ giảm sát thương)
5. Run (Bỏ chạy nếu sự kiện cho phép)

------------------------------------------------------------------------

## 8. Item & Inventory

``` mermaid
classDiagram
    class Item {
        -string id
        -string name
        -string description
        -ItemType type
        -int statValue
    }

    Item <|-- Weapon
    Item <|-- Armor
    Item <|-- Potion
    Inventory o-- Item
```

### Danh sách Key Items Cốt truyện (Fractured Crown)
- **`RoyalInsignia`** (Huy hiệu Hoàng Gia - Node 01): Nhận từ Nhà Vua, giúp qua các trạm gác Hoàng gia.
- **`FreeForestAmulet`** (Bùa Hộ Mệnh Rừng Sâu - Node 06): Vật phẩm bắt buộc để mở khóa Nhánh Lựa Chọn 4 (Bộ Tộc Tự Do - Node 21).
- **`DemonEmpathyRune`** (Cổ Phù Thấu Cảm - Node 08): Giúp giao tiếp hòa bình với Ma Vương và Elena tại Node 10/11 mà không cần giao tranh.
- **`AncientCodex`** (Cổ Thư The Core - Node 07): Chứa tri thức cổ 500 năm về The Core, điều kiện tiên quyết cho True Ending.
- **`ElenaDiary`** (Nhật Ký Elena - Node 11): Tiết lộ bí mật hiến tế linh hồn của The Core, cùng AncientCodex mở khóa True Ending.
- **`GreaterHealthPotion`** / **`HealthPotion`**: Bình thuốc hồi phục sinh lực.

------------------------------------------------------------------------

## 9. Progression

`LevelSystem` xử lý tính toán EXP, thăng cấp và tăng chỉ số cơ bản (HP, MP, ATK, DEF) theo từng Class Hero.

------------------------------------------------------------------------

## 10. Story System & Narrative (Fractured Crown)

### 10.1. Bối cảnh Cốt truyện
Vương quốc Eldoria đang đứng trước bờ vực suy tàn. Nguồn năng lượng ma thuật duy trì sự sống của vương quốc – **The Core** – đang dần cạn kiệt. Đúng đêm Trăng Máu (**Blood Moon**), Công chúa Elena biến mất. Nhà Vua tuyên bố Ma Vương đã bắt cóc nàng và triệu tập chiến binh đi giải cứu. Tuy nhiên, sự thật là The Core đòi hỏi linh hồn của người mang dòng máu hoàng gia hiến tế mỗi 500 năm, và Elena đã tự nguyện bỏ trốn cùng Ma tộc để tìm cách chấm dứt nghi thức tàn bạo này.

### 10.2. Cấu trúc 4 Thế lực
1. 👑 **Kingdom (Vương Quốc)**: Muốn duy trì trật tự và sự thịnh vượng hiện tại bằng mọi giá (kể cả hiến tế).
2. ☠️ **Demon Clan (Ma Tộc)**: Bị con người đày ải và săn đuổi, muốn giải phóng vùng đất và vạch trần tội ác hoàng gia.
3. 🔮 **Mage Council (Hội Đồng Pháp Sư)**: Muốn thâu tóm quyền kiểm soát The Core để lập nên đế chế ma pháp độc tài.
4. 🌿 **Free People (Các Bộ Tộc Tự Do)**: Muốn phá hủy hoặc phong ấn vĩnh viễn The Core để thế giới trở về quy luật tự nhiên.

### 10.3. Sơ đồ Mermaid Flowchart (22 Nodes & 5 Endings)

``` mermaid
flowchart TD
    %% Khởi tạo Hero
    Start([Bắt đầu New Game]) --> ChooseHero{Chọn Lớp Hero}
    ChooseHero -->|Warrior| H_Warrior["Warrior (Arthur)<br/>HP/DEF cao, Cận chiến vững chắc"]
    ChooseHero -->|Mage| H_Mage["Mage (Morrigan)<br/>MP cao, Sát thương phép & Khắc chế ma thuật"]
    ChooseHero -->|Ranger| H_Ranger["Ranger (Lyra)<br/>Nhanh nhẹn, Crit/Dodge, Do thám đường tắt"]

    %% Hồi 1: Chiếu chỉ & Xuất phát
    H_Warrior & H_Mage & H_Ranger --> Node01["Node 01: [STORY] Hoàng Thành Eldoria<br/>Nhận chiếu chỉ từ Nhà Vua & Nhận Item: RoyalInsignia"]
    Node01 --> Node02["Node 02: [STORY] Ngã Ba Biên Giới Trăng Máu"]

    %% Nhánh khám phá Hồi 1
    Node02 -->|Hướng Rừng Sâu Bộ Tộc Tự Do| Node03["Node 03: [COMBAT] Rừng Rậm Huyết Nguyệt<br/>Đánh Minion: Wild_Mercenary"]
    Node02 -->|Hướng Phế Tích Tháp Pháp Sư| Node04["Node 04: [STORY] Tàn Tích Pháp Viện"]
    Node02 -->|Đột kích Tiền đồn Ma Tộc| Node05["Node 05: [COMBAT] Tiền Đồn Ma Tộc<br/>Đánh Minion: Demon_Scout"]

    %% Tương tác Item & Hero Checks
    Node03 -->|Thắng trận| Node06["Node 06: [REWARD] Tộc Trưởng Trao Tặng<br/>Nhận Item: FreeForestAmulet + GreaterHealthPotion"]
    Node04 -->|Hero Mage hoặc Skill Check| Node07["Node 07: [REWARD] Giải Mã Thư Tịch Cổ<br/>Nhận Item: AncientCodex (Cổ Thư The Core)"]
    Node05 -->|Thắng trận| Node08["Node 08: [REWARD] Thu Thập Manh Mối<br/>Nhận Item: DemonEmpathyRune"]

    %% Hồi 2: Thâm nhập & Khám phá Bí Mật The Core
    Node06 & Node07 & Node08 --> Node09["Node 09: [STORY] Thung Lũng Răng Quỷ (Devil's Gorge)"]
    
    Node09 -->|Tiến vào Lâu đài Ma Vương| Node10["Node 10: [REQUIREMENT_CHECK] Kiểm tra Item"]
    Node10 -->|Có DemonEmpathyRune| Node11["Node 11: [STORY] Hội Kiến Ma Vương & Công Chúa Elena<br/>Nhận Item: ElenaDiary (Hiểu rõ chân tướng The Core)"]
    Node10 -->|Không có Rune / Giao chiến trực diện| Node12["Node 12: [COMBAT] Phá Vỡ Cấm Vệ Quỷ<br/>Đánh Minion: Demon_Berserker"]
    Node12 -->|Thắng trận| Node11

    %% Hồi 3: Ngã Rẽ Quyết Định Lớn (4 Phe)
    Node11 --> Node13{"Node 13: [STORY/CHOICE]<br/>Lựa Chọn Của Người Chơi"}

    %% ------------------- NHÁNH 1: PHE VƯƠNG QUỐC -------------------
    Node13 -->|Lựa chọn 1: Bắt Elena nộp cho Nhà Vua| Route_Kingdom["Node 14: [COMBAT] Trảm Ma Vương<br/>Boss: Demon_King_Malakor"]
    Route_Kingdom -->|Thắng trận| Node15["Node 15: [STORY] Hộ Tống Elena Về Eldoria"]
    Node15 --> End1["★ ENDING 1: Vương Quyền Xiềng Xích (Crown of Chains)<br/>(Elena bị hiến tế - Vương quốc duy trì trong giả dối)"]

    %% ------------------- NHÁNH 2: PHE ELENA & MA TỘC -------------------
    Node13 -->|Lựa chọn 2: Cùng Elena & Ma Tộc phá The Core| Route_Rebel["Node 16: [STORY] Tiến Vào Buồng The Core"]
    Route_Rebel --> Node17["Node 17: [COMBAT] Cản đường bởi Đại Tướng Hoàng Gia<br/>Boss: General_Aldric"]
    Node17 -->|Thắng trận| Node18["Node 18: [COMBAT] Kích Hoạt Cấm Vệ Thần<br/>Boss: The_Core_Guardian"]
    
    %% Kiểm tra điều kiện Secret Ending
    Node18 -->|Thắng trận| CheckSecret{"Kiểm tra Item:<br/>AncientCodex + ElenaDiary?"}
    CheckSecret -->|Không đủ item: Phá hủy hoàn toàn Core| End2["★ ENDING 2: Kỷ Nguyên Mới (Dawn of Unity)<br/>(Core vỡ - Vương quốc mất ma thuật - Hòa bình đa tộc)"]
    CheckSecret -->|Có đủ cả 2 Cổ Vật| End5["★ TRUE ENDING: Khúc Ca Hòa Hợp (The Harmonious Resonance)<br/>(Thanh tẩy Core - Hòa bình vĩnh cửu không cần hiến tế)"]

    %% ------------------- NHÁNH 3: PHE HỘI ĐỒNG PHÁP SƯ -------------------
    Node13 -->|Lựa chọn 3: Bắt tay Mage Council thâu tóm The Core| Route_Mage["Node 19: [COMBAT] Tranh Đoạt Trận Địa Pháp Thuật<br/>Boss: Arcane_Council_Enforcers"]
    Route_Mage -->|Thắng trận| Node20["Node 20: [COMBAT] Thanh Trừng Đại Pháp Sư<br/>Boss: Archmage_Morvath"]
    Node20 -->|Thắng trận| End3["★ ENDING 3: Đế Chế Ma Pháp Độc Tài (Arcane Tyranny)<br/>(Mage Council độc chiếm Core, cai trị bằng bạo quyền)"]

    %% ------------------- NHÁNH 4: PHE BỘ TỘC TỰ DO -------------------
    Node13 -->|Lựa chọn 4: Theo Free People phong ấn vĩnh viễn Core| Route_Free["Node 21: [REQUIREMENT_CHECK]<br/>Kiểm tra Item: FreeForestAmulet"]
    Route_Free -->|Thành công| Node22["Node 22: [COMBAT] Loạn Chiến 4 Phe<br/>Boss: Multi-Faction Battle"]
    Node22 -->|Thắng trận| End4["★ ENDING 4: Tự Do Vang Vọng (Echoes of Freedom)<br/>(Core bị phong ấn - Eldoria phân rã thành các bộ tộc tự do)"]

    %% Bad Ending nhánh Combat
    Route_Kingdom -.->|Thất bại trận đấu| GameOver["💀 GAME OVER: Bạn đã ngã xuống nơi chiến trường"]
    Route_Rebel -.->|Thất bại trận đấu| GameOver
    Route_Mage -.->|Thất bại trận đấu| GameOver
    Route_Free -.->|Thất bại trận đấu| GameOver
```

### 10.4. Bảng Ma Trận Tương Tác Giữa Các Thành Phần

| Giai đoạn / Node | Thành phần tham gia | Tương tác & Điều kiện (Requirement / Logic) | Kết quả / Phần thưởng |
| :--- | :--- | :--- | :--- |
| **Bắt đầu** | Hero: `Warrior`, `Mage`, `Ranger` | Chọn 1 trong 3 lớp nhân vật | Khởi tạo chỉ số & kỹ năng riêng |
| **Node 01** | Hero + Hoàng Gia | Nhận chiếu chỉ từ Nhà Vua | Nhận Key Item: `RoyalInsignia` |
| **Node 02** | Hero | Lựa chọn 1 trong 3 hướng đi Biên Giới Trăng Máu | Mở khóa Node 03, 04, hoặc 05 |
| **Node 03** | Hero vs Minion `Wild_Mercenary` | Trận chiến Turn-based tại Rừng Rậm | Thắng $\rightarrow$ Node 06 |
| **Node 04** | Hero: `Mage` hoặc Skill Check | Kiểm tra class `Mage` hoặc điểm kỹ năng | Thành công $\rightarrow$ Node 07 |
| **Node 05** | Hero vs Minion `Demon_Scout` | Trận chiến Turn-based tại Tiền Đồn Ma Tộc | Thắng $\rightarrow$ Node 08 |
| **Node 06** | Hero + Tộc Trưởng Rừng Sâu | Trao thưởng sau khi chứng minh thực lực | Nhận `FreeForestAmulet` + Potion |
| **Node 07** | Hero + Tháp Viện Cổ | Giải mã văn tự cổ ngữ | Nhận Key Item: `AncientCodex` |
| **Node 08** | Hero + Trinh Sát Quỷ | Thu thập chiến lợi phẩm | Nhận Key Item: `DemonEmpathyRune` |
| **Node 09** | Hero | Thâm nhập Thung Lũng Răng Quỷ | Chuyển tiếp tới Node 10 |
| **Node 10** | Item: `DemonEmpathyRune` | Kiểm tra sở hữu `DemonEmpathyRune` | Có Rune $\rightarrow$ Node 11; Không Rune $\rightarrow$ Node 12 |
| **Node 11** | Hero + Elena + Ma Vương | Hội đàm hòa bình, Elena trao nhật ký | Nhận `ElenaDiary`, mở khóa Node 13 |
| **Node 12** | Hero vs Minion `Demon_Berserker` | Trận chiến đột kích trực diện Ma Điện | Thắng $\rightarrow$ Node 11 |
| **Node 13** | Hero (Lựa chọn 4 ngã rẽ) | Chọn phe: Vương Quốc, Elena/Quỷ, Pháp Sư, Bộ Tộc | Mở khóa Node 14, 16, 19, hoặc 21 |
| **Node 14** | Hero vs Boss `Demon_King_Malakor` | Quyết đấu Ma Vương | Thắng $\rightarrow$ Node 15 $\rightarrow$ **Ending 1** |
| **Node 16-17** | Hero vs Boss `General_Aldric` | Đại Tướng cản đường buồng Core | Thắng $\rightarrow$ Node 18 |
| **Node 18** | Hero vs Boss `The_Core_Guardian` | Đấu Cỗ Máy Vệ Thần bảo vệ Core | Thắng $\rightarrow$ Kiểm tra Cổ vật |
| **Check Secret** | Item: `AncientCodex` + `ElenaDiary` | Kiểm tra đủ cả 2 Key Items | Đủ $\rightarrow$ **True Ending**; Thiếu $\rightarrow$ **Ending 2** |
| **Node 19-20** | Hero vs Boss `Archmage_Morvath` | Tranh đoạt Core và diệt Pháp Sư Trưởng | Thắng $\rightarrow$ **Ending 3** |
| **Node 21-22** | Item: `FreeForestAmulet` + Boss `Multi-Faction` | Check Bùa Rừng Sâu và Loạn chiến 4 phe | Thắng $\rightarrow$ **Ending 4** |
| **Combat Nodes** | Bất kỳ trận đấu nào | Hero HP $\le 0$ | Dẫn đến **💀 GAME OVER** |

### 10.5. Ý nghĩa 5 Kết cục (Endings)
- **Ending 1: Vương Quyền Xiềng Xích (*Crown of Chains*)**: Elena bị hiến tế, vương quốc tiếp tục phồn vinh giả dối thêm 500 năm.
- **Ending 2: Kỷ Nguyên Mới (*Dawn of Unity*)**: The Core bị phá hủy, mất ma lực nhưng xóa bỏ ách hiến tế, mở ra hòa bình giữa Người và Ma tộc.
- **Ending 3: Đế Chế Ma Pháp Độc Tài (*Arcane Tyranny*)**: Hội Đồng Pháp Sư đoạt lấy Core, cai trị thế giới bằng bạo quyền ma thuật.
- **Ending 4: Tự Do Vang Vọng (*Echoes of Freedom*)**: The Core bị phong ấn vĩnh viễn, thế giới phân rã thành các bộ tộc tự do nguyên thủy.
- **★ TRUE ENDING: Khúc Ca Hòa Hợp (*The Harmonious Resonance*)**: Dùng tri thức từ `AncientCodex` và ý chí của Elena thanh tẩy The Core thành nguồn năng lượng tuần hoàn tự nhiên vĩnh cửu.

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
