# FEATURE 5: CombatEngine (Turn-based, Status Effects, Damage Formula)
**Thành viên phụ trách:** Sang

---

## Thông tin chung

| Hạng mục | Chi tiết |
| :--- | :--- |
| **Feature Name** | CombatEngine |
| **Thành viên** | Sang |
| **File mã nguồn trọng tâm** | `CombatEngine.h`, `CombatEngine.cpp` (hoặc `CombatEngine.hpp`, `CombatEngine.cpp`) |
| **File Unit Test** | `tests/test_combat.cpp` |
| **Trạng thái** | **Đã hoàn thành** |

---

## Nội dung báo cáo chi tiết

### 1. Vòng lặp chiến đấu theo lượt (Turn-based Loop)
* **Gợi ý nội dung:** Trình bày sơ đồ/logic vòng lặp lượt đi giữa Hero và Enemy. Xử lý hàng đợi hành động: Tấn công thường, Dùng kỹ năng, Dùng Potion, Bỏ chạy (Escape).
* **Nội dung báo cáo:**
  Hệ thống chiến đấu được triển khai theo mô hình State Machine kết hợp vòng lặp theo hiệp (Round-based Loop) thông qua hàm `runBattleLoop()` và bộ điều phối `executeTurn()`:
  - **Pipeline đầu lượt (Start-of-turn Phase):** Trước khi người chơi hoặc quái vật hành động, hàm `processStatusEffects()` được kích hoạt tự động để giải quyết các hiệu ứng theo thời gian:
    - *Tick sát thương Poison DoT:* Trừ máu trực tiếp của thực thể bị nhiễm độc qua `takePoisonDamage()` và giảm bộ đếm `poisonTurns--`. Sát thương được nhân theo số tầng cộng dồn (`poisonDamagePerTurn * poisonStacks`).
    - *Tick hồi phục Regeneration HoT:* Hồi máu cho thực thể qua `processRegen()` và giảm `regenTurns--`.
    - Ngay sau khi tick hiệu ứng, engine kiểm tra sinh tử: nếu một trong hai bên cạn kiệt HP sẽ kết thúc combat ngay mà không kích hoạt các hành động thừa.
  - **Phân định lượt đi dựa trên chỉ số Speed / Agility (Turn Order & Initiative):**
    - Mỗi thực thể sở hữu chỉ số `Speed` riêng biệt (Ranger = 15, Mage = 10, Warrior = 8; Quái thường = 8, Boss = 12-16).
    - Khi bắt đầu hiệp, engine so sánh tốc độ:
      - Nếu `enemy.getSpeed() > hero.getSpeed()`: Kẻ địch chiếm ưu thế tốc độ và giành quyền ra đòn phủ đầu trước (`processEnemyTurn()`). Nếu người chơi sống sót sau đòn đánh này, người chơi mới tiến hành lượt phản công.
      - Nếu `hero.getSpeed() >= enemy.getSpeed()`: Người chơi giữ quyền chủ động hành động trước, sau đó mới đến lượt kẻ địch.
  - **Lượt người chơi (Player Action Phase):** Người chơi lựa chọn 1 trong 5 hành động:
    1. *Tấn công thường (Normal Attack):* Tính toán sát thương cơ bản và trừ HP của mục tiêu.
    2. *Kỹ năng (Skill):* Kiểm tra MP, thời gian hồi chiêu (Cooldown); nếu hợp lệ sẽ thi triển chiêu thức đặc trưng của Class (ví dụ: Chiến binh dùng *Shield Block* áp hiệu ứng Choáng `applyStun(1)`; Cung thủ dùng *Poison Arrow* tiêm độc `applyPoison(3, 12)`).
    3. *Dược phẩm (Potion):* Truy xuất vào túi đồ `Inventory`, xác thực vật phẩm thuộc type `ItemType::POTION`, hồi phục HP/MP tương ứng và loại bỏ item khỏi túi đồ.
    4. *Phòng thủ (Defend):* Kích hoạt thế thủ `isDefending = true`, giảm 50% lượng sát thương thực nhận từ đòn đánh kế tiếp của quái.
    5. *Bỏ chạy (Flee):* Thiết lập trạng thái `CombatState::FLED`, bảo toàn mạng sống và thoát khỏi combat an toàn.
  - **Lượt kẻ địch (Enemy Action Phase):** Nếu quái vật còn sống, hàm `processEnemyTurn()` được gọi:
    - *Kiểm tra khống chế:* Nếu quái dính Choáng (`isStunned()`), quái mất lượt hoàn toàn và giảm bộ đếm choáng `stunTurns--`.
    - *AI hành động:* Quái gọi `chooseAction()` để chọn đòn đánh: Đánh thường (Normal), Đòn giáng búa ngàn cân (Heavy - 1.5x ATK), hoặc Kỹ năng ma thuật của Boss (Boss Special - 1.2x ATK kèm tăng Xuyên giáp).
    - *Tương tác thế thủ của Hero:* Kiểm tra các thế phản xạ của người chơi trước khi trừ máu: Phản đòn (*Parry*) phản ngược 150% sát thương, Chặn đòn (*Block*) triệt tiêu 100% sát thương (0 dmg), Né tránh (*Evade*) né 100% sát thương và nạp thêm 5 Mũi tên, Thế thủ (*Defend*) giảm 50% sát thương.
    - Quái có đặc tính độc (`isPoisonous`) khi đánh trúng sẽ tiêm độc sang người chơi.
  - **Pipeline cuối lượt (End-of-round Phase) & Dọn dẹp trạng thái:**
    - *Hủy hiệu ứng thế thủ:* Gọi `hero.resetCombatStances()` để dọn dẹp các trạng thái phòng ngự tạm thời (Parry, Block, Evade, Defend) vốn chỉ có hiệu lực trong 1 lượt.
    - *Giảm Cooldown:* Gọi `hero.reduceCooldowns()` để đếm lùi thời gian hồi chiêu của các kỹ năng.
    - *Cơ chế chống lặp vô tận (Stalemate / Max Turns):* Nếu 2 bên không gây sát thương trong 5 lượt liên tiếp, hiệu ứng kiệt sức (*Fatigue*) sẽ trừ máu cả hai. Nếu trận đánh vượt quá `MAX_BATTLE_TURNS` (100 lượt), engine sẽ tự động phân định thắng/thua dựa trên % HP còn lại.

---

### 2. Công thức tính sát thương toàn diện (Damage Formula)
* **Gợi ý nội dung:**
  - Công thức tính sát thương gây ra: Sát thương thuần, giảm trừ qua Giáp (Armor), chỉ số Xuyên giáp (Armor Penetration).
  - Công thức tính tỉ lệ bạo kích (Crit Rate & Crit Damage) và tỉ lệ né tránh (Dodge Chance).
* **Nội dung báo cáo:**
  Công thức tính sát thương được đóng gói độc lập và tái sử dụng linh hoạt qua phương thức:
  `DamageResult CombatEngine::calculateDamage(int attackerAttack, float critChance, float critDamage, int armorPen, int defenderDefense, bool ignoreArmor) const;`

  Quy trình tính toán cụ thể:
  1. **Tính Giáp hiệu dụng (Effective Defense):**
     $$\text{EffectiveDefense} = \begin{cases} 0 & \text{nếu } \text{ignoreArmor} = \text{true (Pháp sư / Kỹ năng bỏ qua giáp)} \\ \max(0, \text{defenderDefense} - \text{armorPen}) & \text{nếu tính giáp thông thường} \end{cases}$$
     *Lưu ý bảo vệ biên:* Giáp hiệu dụng luôn được chặn dưới ở mức 0 (`std::max(0, ...)`), ngăn chặn việc chỉ số Xuyên giáp cao hơn Giáp biến thành sát thương cộng dồn bất hợp lý.
  2. **Tính Sát thương thô trước bạo kích (Raw Damage):**
     $$\text{RawDamage} = \max(1, \text{attackerAttack} - \text{EffectiveDefense})$$
     - Áp dụng cơ chế giảm trừ tuyến tính kết hợp **Nguyên tắc Sát thương tối thiểu (Minimum 1 Damage guarantee)**: Bất kỳ đòn đánh nào trúng đích đều gây tối thiểu 1 sát thương (`std::max(1, ...)`). Điều này loại bỏ hoàn toàn lỗi toán học sát thương âm (Edge case: Defense >> Attack làm tăng máu kẻ địch).
  3. **Tính toán Bạo kích (Critical Multiplier):**
     - Sinh số ngẫu nhiên $roll \in [0.0, 1.0]$. Nếu $roll \le \text{critChance}$:
       $$\text{FinalDamage} = \lfloor \text{RawDamage} \times (1.0 + \text{critDamage}) \rfloor$$
       Đồng thời gắn cờ `isCrit = true` để Battle UI hiển thị thông báo hiệu ứng bạo kích.
  4. **Tỷ lệ né tránh (Dodge) và Giảm trừ từ Thế thủ (Defend Reduction):**
     - Kỹ năng né tránh (Ranger Evade) triệt tiêu hoàn toàn sát thương nhận vào ($\text{Damage} = 0$).
     - Khi người chơi kích hoạt thế thủ (*Defend*), sát thương thực nhận được chia đôi tuyến tính:
       $$\text{Damage} = \max\left(1, \left\lfloor \frac{\text{Damage}}{2} \right\rfloor\right)$$
  5. **Buff nguyên tố & Đòn đánh quái vật:**
     - Lớp nhân vật Pháp sư (Mage) sử dụng sát thương phép bỏ qua hoàn toàn giáp vật lý (`ignoreArmor = true`).
     - Đòn đánh nặng của quái (Heavy Attack) nhân hệ số $1.5\times \text{BaseAtk}$; Kỹ năng đặc biệt của Boss (Special Attack) nhân $1.2\times \text{BaseAtk}$ và được cộng thêm $+10$ Armor Penetration.

---

### 3. Hệ thống hiệu ứng trạng thái (StatusEffect)
* **Gợi ý nội dung:**
  - Cấu trúc Struct `StatusEffect`: BURN, FREEZE, POISON, STUN (mất lượt), BLEED, DEFENSE_BUFF.
  - Cơ chế giảm số lượt hiệu ứng (Duration countdown) và áp dụng sát thương theo thời gian (DoT tick) ở đầu mỗi lượt.
* **Nội dung báo cáo:**
  Hệ thống hiệu ứng được định nghĩa trong `StatusEffect.h` với enum `StatusType` (bao gồm các hiệu ứng cốt lõi: POISON, REGENERATION, STUN và mở rộng cho giai đoạn tiếp theo: BURN, FREEZE, BLEED, DEFENSE_BUFF) cùng struct dữ liệu:
  ```cpp
  struct StatusEffect {
      StatusType type = StatusType::NONE;
      std::string name;
      int duration = 0; // Số lượt hiệu lực còn lại
      int value = 0;    // Giá trị sát thương (DoT) hoặc hồi phục (HoT) mỗi lượt
      int stacks = 1;   // Số tầng hiệu ứng cộng dồn
      static constexpr int MAX_STACKS = 3; // Giới hạn tối đa 3 tầng
  };
  ```
  - **Cơ chế đếm lùi thời lượng (Duration Countdown) & DoT Tick:**
    - Được kích hoạt ở đầu hiệp đấu qua hàm `processStatusEffects()`.
    - Sát thương DoT (Poison/Bleed) là sát thương thuần bỏ qua giáp, tác động trực tiếp vào HP qua `takePoisonDamage()`. Sau mỗi lần tick, số lượt giảm 1 (`poisonTurns--`). Khi số lượt về 0, giá trị sát thương và số stack được reset về 0 và hiệu ứng tự động kết thúc.
    - Tương tự, HoT (Regeneration) kích hoạt `processRegen()` hồi phục HP (kẹp không vượt quá `maxHp`) và giảm `regenTurns--`.
  - **Cơ chế Cộng dồn & Làm mới thời lượng (Stacking / Refresh Duration):**
    - *Đối với hiệu ứng Khống chế (Crowd Control - STUN):* Áp dụng nguyên tắc **Giữ thời lượng lớn nhất (Max Duration Retention / Refresh)**:
      ```cpp
      stunTurns = std::max(stunTurns, turns);
      ```
      Cơ chế này đảm bảo khi mục tiêu đang bị choáng mà nhận thêm một kỹ năng làm choáng mới, thời lượng choáng sẽ được làm mới về giá trị lớn hơn chứ không bị ghi đè giảm đi hay tích lũy vô hạn gây vỡ trận đấu (Permastun break).
    - *Đối với hiệu ứng Sát thương theo thời gian (DoT - POISON / BLEED):*
      Áp dụng cơ chế **Cộng dồn số tầng (Stacking up to 3 stacks)** kết hợp **Làm mới thời lượng (Refresh Duration)**:
      ```cpp
      poisonStacks = std::min(3, poisonStacks + stackInc);
      poisonTurns = std::max(poisonTurns, turns);
      poisonDamagePerTurn = std::max(poisonDamagePerTurn, damagePerTurn);
      ```
      Khi đó, sát thương DoT mỗi lượt được khuếch đại:
      $$\text{TotalDoTDamage} = \text{poisonDamagePerTurn} \times \text{poisonStacks}$$
    - *Dọn dẹp trạng thái:* Khi `duration == 0`, cờ trạng thái `isPoisoned()` / `isStunned()` tự động trả về `false`, không để lại trạng thái treo (dangling state).

---

### 4. Điều kiện kết thúc trận đánh & Trao thưởng
* **Gợi ý nội dung:** Phân định kết quả Thắng/Thua (Win/Loss), logic phát thưởng EXP/Vàng/Item, và chuyển hướng giao diện sau trận đánh.
* **Nội dung báo cáo:**
  - **Phân định kết quả (Combat State Resolution):**
    - `HERO_VICTORY`: Kích hoạt ngay khi HP của quái vật rơi về 0 (`!enemy.isAlive()`), bất kể do đòn đánh thường, kỹ năng bộc phá, phản đòn hay chết vì trúng DoT độc ở đầu lượt.
    - `ENEMY_VICTORY`: Kích hoạt khi HP của người chơi về 0 (`!hero.isAlive()`).
    - `FLED`: Kích hoạt khi người chơi chọn rút lui thành công khỏi trận đấu.
    - `STALEMATE TIMEOUT`: Trận đấu quá 100 lượt sẽ tự động phân định dựa trên so sánh tỷ lệ `% HP` sinh tồn của hai bên.
  - **Logic phát thưởng tự động (`processVictoryRewards`):**
    - *EXP & Vàng:* Cộng trực tiếp cho người chơi qua `hero.addExp(enemy.getExpReward())` (tự động kích hoạt logic Level-up nếu vượt ngưỡng) và `hero.addGold(enemy.getGoldReward())`.
    - *Rơi vật phẩm (Loot Drops):* Gọi `enemy.generateLootDrops()` để tính toán tỷ lệ rơi đồ dựa trên `dropChance`. Sau đó, `DataLoader::loadItemById()` sẽ nạp thông tin item từ `data/items.json` và đưa vào túi đồ của người chơi thông qua `inventory->addItem(itm)`.
    - *Xử lý đầy túi đồ (Full Inventory):* Nếu túi đồ đã đầy (`targetInv->addItem()` trả về `false`), hệ thống sẽ thông báo rõ ràng cho người chơi mà không làm mất trạng thái hay crash game. Danh sách vật phẩm rơi được lưu trữ an toàn trong `lastLootDrops` để hỗ trợ kiểm thử và log giao diện.
  - **Chuyển hướng giao diện sau trận:**
    - Sau khi thoát khỏi vòng lặp `runBattleLoop()`, engine in banner tổng kết kết quả trận đấu kèm chi tiết thưởng nhận được.
    - Quyền điều khiển được hoàn trả cho `GameManager`: Nếu chiến thắng, trò chơi kích hoạt nhánh tiếp theo trên Cây cốt truyện (`StoryGraph`); nếu thất bại, màn hình điều hướng người chơi đến Game Over Menu (cho phép tải lại file lưu thông qua `SaveManager` hoặc quay về Main Menu).

---

### 5. An toàn bộ nhớ
* **Gợi ý nội dung:** Cách kiểm tra đối tượng còn sống (`isAlive()`) trước khi tính toán tác động, tránh truy cập vào con trỏ rác hay con trỏ hỏng (Dangling Pointer).
* **Nội dung báo cáo:**
  Kiến trúc `CombatEngine` được thiết kế chú trọng tuyệt đối vào tính an toàn bộ nhớ của C++:
  - **Sử dụng Reference & Smart Pointer:**
    - Hai thực thể tham gia giao tranh được lưu trữ dưới dạng C++ References (`Hero& hero`, `Enemy& enemy`). Điều này loại trừ hoàn toàn khả năng xuất hiện con trỏ rỗng (`nullptr`) hay lỗi truy cập con trỏ rác (Dangling Pointer) trong suốt vòng đời của trận đánh.
    - Túi đồ và vật phẩm rơi sử dụng `std::shared_ptr<Item>` kết hợp với `DataLoader`, đảm bảo bộ nhớ heap được cấp phát và giải phóng tự động thông qua cơ chế đếm tham chiếu (Reference Counting), triệt tiêu nguy cơ rò rỉ bộ nhớ (Memory Leak).
    - Biến con trỏ `Inventory* inventory` luôn được kiểm tra an toàn bằng toán tử ba ngôi: `Inventory* targetInv = inventory ? inventory : &hero.getInventory();` cùng các bước kiểm tra `if (!targetInv || targetInv->getItemCount() == 0)` trước khi thao tác.
  - **Kiểm tra sinh tử (`isAlive()`) đa tầng:**
    - Luôn kiểm tra `isAlive()` ngay sau phase tick DoT độc đầu hiệp. Nếu quái gục ngã vì độc, engine lập tức kích hoạt `processVictoryRewards()` và `return`, ngăn chặn hoàn toàn việc người chơi cố tác động sát thương lên một đối tượng đã chết.
    - Kiểm tra `enemy.isAlive()` trước khi cho phép quái bước vào lượt hành động trong `processEnemyTurn()`.
    - Kiểm tra `hero.isAlive()` trước khi áp dụng hiệu ứng tiêm độc từ quái (`if (enemy.getIsPoisonous() && hero.isAlive())`).
  - **Bảo vệ chỉ số biên (Clamping & Bounds Safety):**
    - Các phương thức `takeDamage()`, `takeDirectDamage()`, `heal()` trên thực thể luôn sử dụng `std::clamp` / `std::max` / `std::min` để đảm bảo HP không bao giờ bị âm dưới 0 và không vượt quá `maxHp`.
    - Mọi thao tác chọn Potion trong túi đồ đều kiểm tra chỉ số biên mảng `[0, getItemCount() - 1]` để ngăn chặn lỗi Out-of-bounds Access.

---

### 6. Unit Test chứng minh
* **Gợi ý nội dung:** Tóm tắt các trường hợp test vòng lặp combat, tính sát thương và status effect trong `tests/test_combat.cpp`.
* **Nội dung báo cáo:**
  File kiểm thử trọng tâm `tests/test_combat.cpp` bao gồm 14 kịch bản test case toàn diện nhằm xác minh tính đúng đắn của logic chiến đấu và các trường hợp biên ngặt nghèo:
  1. *Test Case 1 - Biên sát thương âm (Defense >> Attack):* Kiểm tra trường hợp Hero có ATK = 5 đánh Enemy có DEF = 999. Hiệu số $5 - 999 = -994$ được kẹp về đúng 1 damage tối thiểu; HP địch giảm từ 100 xuống 99, khẳng định sát thương không bao giờ bị âm làm hồi máu đối phương.
  2. *Test Case 2 - Xuyên giáp & Bỏ qua giáp:* Xác minh Xuyên giáp lớn hơn Giáp không tạo bonus sát thương vô lý (Pen 30 vs Def 10 thì giáp hiệu dụng = 0). Xác minh cờ `ignoreArmor = true` của Pháp sư bỏ qua hoàn toàn 999 giáp.
  3. *Test Case 3 - Biên HP không âm & Hồi máu không vượt Max HP:* Nhận 500 sát thương khi máu chỉ có 100 thì HP dừng chính xác ở 0 và `isAlive() == false`. Hồi máu 200 HP thì máu kẹp chính xác ở `maxHp`.
  4. *Test Case 4 - HP về 0 đúng thời điểm trúng DoT (Edge Case):* Mô phỏng quái hoặc người chơi trúng độc Poison DoT ở đầu lượt. Khi máu rút về 0 tại thời điểm tick độc, vòng lặp ngắt ngay tức khắc, kích hoạt trao thưởng chiến thắng chuẩn xác mà không gặp bất kỳ lỗi logic hay crash chương trình nào.
  5. *Test Case 5 - Ranger Poison Arrow:* Kỹ năng gán chính xác 3 lượt độc (12 dmg/lượt). Lượt kế tiếp độc trừ đúng 12 HP và số lượt giảm còn 2.
  6. *Test Case 6 - Regeneration HoT trên Hero:* Kích hoạt hồi máu theo lượt, kiểm tra lượng máu tăng thêm và bộ đếm lượt giảm đều đặn.
  7. *Test Case 7 - Quái hệ độc (Poisonous Enemy):* Xác minh quái có cờ độc tiêm độc thành công sang người chơi khi đánh trúng.
  8. *Test Case 8 - Dùng Dược phẩm (Potion & Mana Potion) trong trận:* Xác minh hồi phục chính xác 35 HP / 30 MP và vật phẩm được trừ khỏi `Inventory`.
  9. *Test Case 9 - Thế thủ, Chặn đòn & Bỏ chạy:* Xác minh Defend giảm 50% sát thương, Warrior Shield Block triệt tiêu 100% sát thương (0 dmg), và hành động Flee chuyển đúng state `CombatState::FLED`.
  10. *Test Case 10 - Chu kỳ AI của Boss:* Kiểm thử Boss ra đòn tuần hoàn chuẩn xác theo từng lượt: Đánh thường $\rightarrow$ Đòn cực mạnh $\rightarrow$ Kỹ năng Boss đặc biệt.
  11. *Test Case 11 - Tự động nhặt vật phẩm chiến lợi phẩm (Loot Drops):* Đánh bại quái có tỷ lệ rơi đồ 100%, kiểm tra Hero tự động nhận đúng Vàng, EXP và 2 trang bị rơi vào túi đồ.
  12. *Test Case 12 - Polymorphic AI của Minion (Wild Mercenary):* Kiểm thử tính đa hình và hành vi lựa chọn chiêu thức của quái vật.
  13. *Test Case 13 - Speed / Agility Turn Initiative:* Kiểm thử phân định lượt đi theo chỉ số Speed: khi Hero có tốc độ cao hơn thì đi trước (`isHeroFirst() == true`), khi Enemy có tốc độ cao hơn thì Enemy giành quyền đi trước (`isHeroFirst() == false`).
  14. *Test Case 14 - Status Effect Stacking (Poison) & Refresh Duration (Stun & Poison):* Kiểm thử cộng dồn độc tố lên đến 3 tầng (sát thương nhân 3x) và kiểm tra làm mới thời lượng lớn nhất cho Stun / Poison.

  - **Kết quả kiểm thử thực tế:**
    Toàn bộ hệ sinh thái kiểm thử của dự án được thực thi tự động qua hệ thống **Docker CTest** với kết quả đạt:
    $$\mathbf{16/16\ Test\ Suites\ Passed\ (100\%)\ trong\ 2.71s}$$
    Không xuất hiện bất kỳ lỗi phân đoạn (Segmentation Fault), vi phạm Assertion hay rò rỉ bộ nhớ nào.
