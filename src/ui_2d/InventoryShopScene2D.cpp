#include "ui_2d/InventoryShopScene2D.h"
#include "Weapon.h"
#include "Armor.h"
#include "Potion.h"
#include <algorithm>
#include <cmath>

InventoryShopScene2D::InventoryShopScene2D(Hero& hero, Shop* shop, InventoryShopMode mode)
    : m_hero(hero)
    , m_shop(shop)
    , m_mode(mode)
    , m_closed(false)
    , m_selectedIndex(0)
    , m_notificationText("")
    , m_notificationTimer(0.0f)
    , m_notificationColor(GREEN)
    , m_animTimer(0.0f)
{
}

void InventoryShopScene2D::open(InventoryShopMode mode) {
    m_mode = mode;
    m_closed = false;
    m_selectedIndex = 0;
    m_notificationTimer = 0.0f;
}

void InventoryShopScene2D::update(float dt) {
    if (m_closed) return;

    m_animTimer += dt;
    if (m_notificationTimer > 0.0f) {
        m_notificationTimer -= dt;
    }

    // Đóng giao diện
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_I) || IsKeyPressed(KEY_B) || IsKeyPressed(KEY_TAB)) {
        m_closed = true;
        return;
    }

    // Chuyển chế độ trong Shop (Tab để đổi giữa Mua & Bán)
    if (m_shop != nullptr) {
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO)) {
            if (m_mode == InventoryShopMode::SHOP_BUY) {
                m_mode = InventoryShopMode::SHOP_SELL;
                m_selectedIndex = 0;
            } else if (m_mode == InventoryShopMode::SHOP_SELL) {
                m_mode = InventoryShopMode::SHOP_BUY;
                m_selectedIndex = 0;
            }
        }
    }

    if (m_mode == InventoryShopMode::INVENTORY_ONLY || m_mode == InventoryShopMode::SHOP_SELL) {
        handleInventoryActions();
    } else if (m_mode == InventoryShopMode::SHOP_BUY) {
        handleShopActions();
    }
}

void InventoryShopScene2D::handleInventoryActions() {
    auto& items = m_hero.getInventory().getItems();
    int itemCount = (int)items.size();

    if (itemCount > 0) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_selectedIndex = (m_selectedIndex - 1 + itemCount) % itemCount;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_selectedIndex = (m_selectedIndex + 1) % itemCount;
        }
        m_selectedIndex = std::max(0, std::min(m_selectedIndex, itemCount - 1));
    }

    if (itemCount > 0 && m_selectedIndex >= 0 && m_selectedIndex < itemCount) {
        auto item = items[m_selectedIndex];

        // 1. Chế độ Mở Túi Đồ Thông Thường
        if (m_mode == InventoryShopMode::INVENTORY_ONLY) {
            // Phím [E] hoặc [ENTER]: Sử dụng / Trang bị vật phẩm
            if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (item->getType() == ItemType::POTION) {
                    if (m_hero.getInventory().useItem(m_selectedIndex, m_hero)) {
                        m_notificationText = "Đã sử dụng " + item->getName() + "!";
                        m_notificationColor = GREEN;
                        m_notificationTimer = 2.0f;
                    } else {
                        m_notificationText = "Không thể sử dụng vật phẩm lúc này!";
                        m_notificationColor = RED;
                        m_notificationTimer = 2.0f;
                    }
                } else if (item->getType() == ItemType::WEAPON) {
                    m_hero.getInventory().equipWeapon(m_selectedIndex);
                    m_notificationText = "Đã trang bị " + item->getName() + "!";
                    m_notificationColor = YELLOW;
                    m_notificationTimer = 2.0f;
                } else if (item->getType() == ItemType::ARMOR) {
                    m_hero.getInventory().equipArmor(m_selectedIndex);
                    m_notificationText = "Đã trang bị " + item->getName() + "!";
                    m_notificationColor = YELLOW;
                    m_notificationTimer = 2.0f;
                }
            }

            // Phím [U]: Tháo trang bị
            if (IsKeyPressed(KEY_U)) {
                if (item->getType() == ItemType::WEAPON && m_hero.getInventory().getEquippedWeaponIndex() == m_selectedIndex) {
                    m_hero.getInventory().unequipWeapon();
                    m_notificationText = "Đã tháo vũ khí!";
                    m_notificationColor = ORANGE;
                    m_notificationTimer = 2.0f;
                } else if (item->getType() == ItemType::ARMOR && m_hero.getInventory().getEquippedArmorIndex() == m_selectedIndex) {
                    m_hero.getInventory().unequipArmor();
                    m_notificationText = "Đã tháo giáp trụ!";
                    m_notificationColor = ORANGE;
                    m_notificationTimer = 2.0f;
                }
            }

            // Phím [X] hoặc [DELETE]: Vứt bỏ vật phẩm
            if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_DELETE)) {
                std::string removedName = item->getName();
                m_hero.getInventory().removeItem(m_selectedIndex);
                m_notificationText = "Đã vứt bỏ " + removedName;
                m_notificationColor = RED;
                m_notificationTimer = 2.0f;
                if (m_selectedIndex >= (int)m_hero.getInventory().getItems().size()) {
                    m_selectedIndex = std::max(0, (int)m_hero.getInventory().getItems().size() - 1);
                }
            }
        }
        // 2. Chế độ Bán Đồ trong Cửa Hàng
        else if (m_mode == InventoryShopMode::SHOP_SELL && m_shop != nullptr) {
            if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                std::string itemName = item->getName();
                if (m_shop->sellItem(m_selectedIndex, m_hero)) {
                    m_notificationText = "Đã bán " + itemName + " thu về vàng!";
                    m_notificationColor = GOLD;
                    m_notificationTimer = 2.0f;
                    if (m_selectedIndex >= (int)m_hero.getInventory().getItems().size()) {
                        m_selectedIndex = std::max(0, (int)m_hero.getInventory().getItems().size() - 1);
                    }
                } else {
                    m_notificationText = "Không thể bán vật phẩm này!";
                    m_notificationColor = RED;
                    m_notificationTimer = 2.0f;
                }
            }
        }
    }
}

void InventoryShopScene2D::handleShopActions() {
    if (!m_shop) return;
    const auto& goods = m_shop->getGoods();
    int goodsCount = (int)goods.size();

    if (goodsCount > 0) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_selectedIndex = (m_selectedIndex - 1 + goodsCount) % goodsCount;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_selectedIndex = (m_selectedIndex + 1) % goodsCount;
        }
        m_selectedIndex = std::max(0, std::min(m_selectedIndex, goodsCount - 1));

        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            const auto& shopItem = goods[m_selectedIndex];
            if (m_hero.getGold() < shopItem.buyPrice) {
                m_notificationText = "Không đủ Vàng để mua!";
                m_notificationColor = RED;
                m_notificationTimer = 2.0f;
            } else if (m_hero.getInventory().isFull()) {
                m_notificationText = "Hành trang đã đầy!";
                m_notificationColor = RED;
                m_notificationTimer = 2.0f;
            } else {
                std::string boughtName = shopItem.item->getName();
                if (m_shop->buyItem(m_selectedIndex, m_hero)) {
                    m_notificationText = "Đã mua thành công " + boughtName + "!";
                    m_notificationColor = GREEN;
                    m_notificationTimer = 2.0f;
                }
            }
        }
    }
}

void InventoryShopScene2D::render() {
    if (m_closed) return;

    // Nền tối mờ phía sau
    DrawRectangle(0, 0, 480, 270, Color{ 0, 0, 0, 195 });

    // 1. Khung Tiêu Đề Chính
    AssetManager& assets = AssetManager::getInstance();
    std::string title = (m_mode == InventoryShopMode::INVENTORY_ONLY) ? "📦 HÀNH TRANG & CHỈ SỐ ANH HÙNG"
                        : (m_mode == InventoryShopMode::SHOP_BUY)     ? "🛒 CỬA HÀNG HOÀNG THÀNH [MUA HÀNG]"
                                                                      : "💰 CỬA HÀNG HOÀNG THÀNH [BÁN ĐỒ]";
    Color titleColor = (m_mode == InventoryShopMode::INVENTORY_ONLY) ? SKYBLUE : GOLD;

    assets.drawNineSliceBox(Rectangle{ 8, 6, 464, 26 }, Color{ 14, 20, 38, 255 }, titleColor);
    assets.drawText(title, 16, 11, 11, titleColor);

    // Hiển thị Vàng ở góc phải
    assets.drawItemIcon("gold", 370, 10, 16);
    assets.drawText(TextFormat("%d Vàng", m_hero.getGold()), 390, 12, 10, GOLD);

    // 2. Bảng Chỉ Số Anh Hùng (Bên Trái)
    renderHeroStatsPanel();

    // 3. Bảng Danh Sách Vật Phẩm (Ở Giữa)
    if (m_mode == InventoryShopMode::INVENTORY_ONLY || m_mode == InventoryShopMode::SHOP_SELL) {
        renderInventoryGrid();
    } else {
        renderShopGrid();
    }

    // 4. Bảng Chi Tiết & Hướng Dẫn Thao Tác (Bên Phải / Đáy)
    renderItemDetailPanel();

    // 5. Thông Báo Nổi
    if (m_notificationTimer > 0.0f) {
        renderNotification();
    }

    // 6. Dải Hướng Dẫn Đáy Màn Hình
    DrawRectangle(0, 254, 480, 16, Color{ 8, 12, 24, 240 });
    std::string hints = (m_mode == InventoryShopMode::INVENTORY_ONLY)
        ? "[W/S/Mũi tên]: Chọn | [E/Enter]: Dùng/Mặc | [U]: Tháo | [X]: Vứt | [ESC/I]: Đóng"
        : "[W/S]: Chọn | [E/Enter]: Giao dịch | [TAB]: Chuyển Mua/Bán | [ESC]: Rời Shop";
    assets.drawText(hints, 14, 257, 8, LIGHTGRAY);
}

void InventoryShopScene2D::renderHeroStatsPanel() {
    AssetManager& assets = AssetManager::getInstance();

    Rectangle statsRect = { 8, 36, 140, 214 };
    assets.drawNineSliceBox(statsRect, Color{ 12, 16, 32, 245 }, Color{ 70, 100, 150, 255 });

    // Avatar Sprite & Tên
    Texture2D heroTex = assets.getHeroTexture(m_hero.getHeroClassName());
    assets.drawEntitySprite(heroTex, 14, 42, 1.2f);

    assets.drawText(m_hero.getName(), 54, 44, 10, YELLOW);
    assets.drawText(TextFormat("Cấp %d (%s)", m_hero.getLevel(), m_hero.getHeroClassName().c_str()), 54, 58, 8, LIGHTGRAY);

    // Thanh Máu HP & Mana MP
    assets.drawText("HP:", 14, 76, 8, WHITE);
    assets.drawStatBar(34, 76, 106, 7, m_hero.getHp(), m_hero.getMaxHp(), Color{ 220, 45, 55, 255 }, Color{ 30, 30, 30, 255 });
    assets.drawText(TextFormat("%d/%d", m_hero.getHp(), m_hero.getMaxHp()), 70, 75, 7, WHITE);

    assets.drawText("MP:", 14, 88, 8, WHITE);
    assets.drawStatBar(34, 88, 106, 7, m_hero.getMp(), m_hero.getMaxMp(), Color{ 35, 130, 245, 255 }, Color{ 30, 30, 30, 255 });
    assets.drawText(TextFormat("%d/%d", m_hero.getMp(), m_hero.getMaxMp()), 70, 87, 7, WHITE);

    // Chỉ số Tấn công & Phòng thủ
    int wepBonus = m_hero.getInventory().getEquippedWeaponBonus();
    int armBonus = m_hero.getInventory().getEquippedArmorBonus();

    assets.drawText(TextFormat("⚔️ Sát thương: %d (+%d)", m_hero.getAttack(), wepBonus), 14, 104, 8, Color{ 255, 140, 140, 255 });
    assets.drawText(TextFormat("🛡️ Phòng thủ:  %d (+%d)", m_hero.getDefense(), armBonus), 14, 118, 8, Color{ 140, 200, 255, 255 });

    // Trang bị hiện tại
    DrawLine(14, 134, 142, 134, Color{ 50, 70, 100, 255 });
    assets.drawText("⚡ TRANG BỊ ĐANG DÙNG:", 14, 138, 8, GOLD);

    auto eqWep = m_hero.getInventory().getEquippedWeapon();
    auto eqArm = m_hero.getInventory().getEquippedArmor();

    // Ô Vũ Khí
    assets.drawItemIcon("sword", 14, 150, 16);
    std::string wepName = eqWep ? eqWep->getName() : "(Trống)";
    assets.drawText(wepName, 34, 154, 8, eqWep ? YELLOW : GRAY);

    // Ô Giáp Trụ
    assets.drawItemIcon("armor", 14, 172, 16);
    std::string armName = eqArm ? eqArm->getName() : "(Trống)";
    assets.drawText(armName, 34, 176, 8, eqArm ? SKYBLUE : GRAY);

    // Sức chứa túi đồ
    int curItems = (int)m_hero.getInventory().getItems().size();
    int maxCap = m_hero.getInventory().getCapacity();
    assets.drawText(TextFormat("📦 Sức chứa: %d/%d", curItems, maxCap), 14, 200, 8, curItems >= maxCap ? RED : WHITE);
}

void InventoryShopScene2D::renderInventoryGrid() {
    AssetManager& assets = AssetManager::getInstance();

    Rectangle gridRect = { 152, 36, 170, 214 };
    assets.drawNineSliceBox(gridRect, Color{ 12, 16, 32, 245 }, Color{ 70, 100, 150, 255 });

    std::string listHeader = (m_mode == InventoryShopMode::INVENTORY_ONLY) ? "DANH SÁCH VẬT PHẨM" : "CHỌN ĐỒ ĐỂ BÁN";
    assets.drawText(listHeader, 160, 42, 9, YELLOW);
    DrawLine(158, 54, 314, 54, Color{ 50, 70, 100, 255 });

    const auto& items = m_hero.getInventory().getItems();
    if (items.empty()) {
        assets.drawText("(Túi đồ rỗng)", 195, 120, 9, GRAY);
        return;
    }

    int startY = 58;
    int slotHeight = 22;
    int maxVisible = 8;
    int scrollOffset = std::max(0, m_selectedIndex - maxVisible + 1);

    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)items.size(); ++i) {
        int idx = i + scrollOffset;
        const auto& item = items[idx];
        int y = startY + i * slotHeight;

        bool isSelected = (idx == m_selectedIndex);
        bool isEquipped = (idx == m_hero.getInventory().getEquippedWeaponIndex() || idx == m_hero.getInventory().getEquippedArmorIndex());

        if (isSelected) {
            float pulse = std::sin(m_animTimer * 8.0f) * 15.0f;
            DrawRectangle(156, y, 162, 20, Color{ 35, 65, 120, (unsigned char)(200 + pulse) });
            DrawRectangleLines(156, y, 162, 20, GOLD);
        } else {
            DrawRectangle(156, y, 162, 20, (idx % 2 == 0) ? Color{ 18, 24, 45, 180 } : Color{ 22, 30, 55, 180 });
        }

        // Icon
        std::string iconId = (item->getType() == ItemType::WEAPON) ? "sword"
                            : (item->getType() == ItemType::ARMOR)  ? "armor"
                            : (item->getType() == ItemType::POTION) ? "potion_red" : "gold";
        assets.drawItemIcon(iconId, 160, y + 2, 16);

        // Tên
        Color itemColor = isEquipped ? GOLD : (isSelected ? WHITE : LIGHTGRAY);
        std::string displayName = item->getName();
        if (displayName.length() > 14) displayName = displayName.substr(0, 12) + "..";
        assets.drawText(displayName, 180, y + 5, 8, itemColor);

        // Nhãn [E] nếu đang trang bị
        if (isEquipped) {
            assets.drawText("[E]", 298, y + 5, 8, YELLOW);
        }
    }
}

void InventoryShopScene2D::renderShopGrid() {
    AssetManager& assets = AssetManager::getInstance();
    if (!m_shop) return;

    Rectangle gridRect = { 152, 36, 170, 214 };
    assets.drawNineSliceBox(gridRect, Color{ 12, 16, 32, 245 }, GOLD);

    assets.drawText("HÀNG HÓA BÀY BÁN", 160, 42, 9, GOLD);
    DrawLine(158, 54, 314, 54, Color{ 80, 70, 40, 255 });

    const auto& goods = m_shop->getGoods();
    if (goods.empty()) {
        assets.drawText("(Hết hàng)", 205, 120, 9, GRAY);
        return;
    }

    int startY = 58;
    int slotHeight = 22;
    int maxVisible = 8;
    int scrollOffset = std::max(0, m_selectedIndex - maxVisible + 1);

    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)goods.size(); ++i) {
        int idx = i + scrollOffset;
        const auto& g = goods[idx];
        int y = startY + i * slotHeight;

        bool isSelected = (idx == m_selectedIndex);
        if (isSelected) {
            DrawRectangle(156, y, 162, 20, Color{ 60, 50, 25, 240 });
            DrawRectangleLines(156, y, 162, 20, GOLD);
        } else {
            DrawRectangle(156, y, 162, 20, (idx % 2 == 0) ? Color{ 22, 24, 35, 180 } : Color{ 28, 30, 45, 180 });
        }

        std::string iconId = (g.item->getType() == ItemType::WEAPON) ? "sword"
                            : (g.item->getType() == ItemType::ARMOR)  ? "armor"
                            : (g.item->getType() == ItemType::POTION) ? "potion_red" : "gold";
        assets.drawItemIcon(iconId, 160, y + 2, 16);

        // Tên & Giá
        std::string name = g.item->getName();
        if (name.length() > 11) name = name.substr(0, 10) + "..";
        assets.drawText(name, 180, y + 5, 8, isSelected ? YELLOW : WHITE);

        // Giá vàng
        assets.drawText(TextFormat("%d G", g.buyPrice), 275, y + 5, 8, GOLD);
    }
}

void InventoryShopScene2D::renderItemDetailPanel() {
    AssetManager& assets = AssetManager::getInstance();

    Rectangle detailRect = { 326, 36, 146, 214 };
    assets.drawNineSliceBox(detailRect, Color{ 12, 16, 32, 245 }, Color{ 70, 100, 150, 255 });

    assets.drawText("CHI TIẾT VẬT PHẨM", 336, 42, 9, YELLOW);
    DrawLine(332, 54, 466, 54, Color{ 50, 70, 100, 255 });

    std::shared_ptr<Item> curItem = nullptr;
    int price = 0;

    if (m_mode == InventoryShopMode::INVENTORY_ONLY || m_mode == InventoryShopMode::SHOP_SELL) {
        const auto& items = m_hero.getInventory().getItems();
        if (!items.empty() && m_selectedIndex >= 0 && m_selectedIndex < (int)items.size()) {
            curItem = items[m_selectedIndex];
            price = curItem->getStatValue() * 3; // Giá bán lại
        }
    } else if (m_mode == InventoryShopMode::SHOP_BUY && m_shop != nullptr) {
        const auto& goods = m_shop->getGoods();
        if (!goods.empty() && m_selectedIndex >= 0 && m_selectedIndex < (int)goods.size()) {
            curItem = goods[m_selectedIndex].item;
            price = goods[m_selectedIndex].buyPrice;
        }
    }

    if (!curItem) {
        assets.drawText("Chưa chọn vật phẩm", 340, 120, 8, GRAY);
        return;
    }

    // Icon lớn ở giữa
    std::string iconId = (curItem->getType() == ItemType::WEAPON) ? "sword"
                        : (curItem->getType() == ItemType::ARMOR)  ? "armor"
                        : (curItem->getType() == ItemType::POTION) ? "potion_red" : "gold";
    assets.drawNineSliceBox(Rectangle{ 380, 62, 36, 36 }, Color{ 20, 26, 48, 255 }, GOLD);
    assets.drawItemIcon(iconId, 390, 72, 16);

    // Tên vật phẩm
    assets.drawText(curItem->getName(), 334, 105, 9, YELLOW);

    // Phân loại
    std::string typeStr = (curItem->getType() == ItemType::WEAPON) ? "Loại: Vũ khí cận chiến"
                        : (curItem->getType() == ItemType::ARMOR)  ? "Loại: Giáp hộ thân"
                        : (curItem->getType() == ItemType::POTION) ? "Loại: Dược phẩm tiêu hao" : "Loại: Kho báu";
    assets.drawText(typeStr, 334, 120, 7, SKYBLUE);

    // Mô tả vật phẩm
    assets.drawText(curItem->getDescription(), 334, 136, 7, LIGHTGRAY);

    // Giá trị
    DrawLine(332, 180, 466, 180, Color{ 50, 70, 100, 255 });
    if (m_mode == InventoryShopMode::SHOP_BUY) {
        assets.drawText(TextFormat("💰 Giá mua: %d Vàng", price), 334, 188, 8, GOLD);
    } else if (m_mode == InventoryShopMode::SHOP_SELL) {
        assets.drawText(TextFormat("💰 Giá bán: %d Vàng", price), 334, 188, 8, GOLD);
    } else {
        assets.drawText(TextFormat("⚡ Chỉ số: +%d", curItem->getStatValue()), 334, 188, 8, GOLD);
    }
}

void InventoryShopScene2D::renderNotification() {
    AssetManager& assets = AssetManager::getInstance();

    float w = assets.measureText(m_notificationText, 9) + 20.0f;
    Rectangle notifBox = { 240.0f - w * 0.5f, 215.0f, w, 22.0f };

    assets.drawNineSliceBox(notifBox, Color{ 10, 14, 26, 245 }, m_notificationColor);
    assets.drawText(m_notificationText, notifBox.x + 10, notifBox.y + 5, 9, m_notificationColor);
}
