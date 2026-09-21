#ifndef INVENTORY_SHOP_SCENE_2D_H
#define INVENTORY_SHOP_SCENE_2D_H

#include "Hero.h"
#include "Shop.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <string>
#include <vector>

enum class InventoryShopMode {
    INVENTORY_ONLY, // Xem hành trang, chỉ số nhân vật, trang bị đồ
    SHOP_BUY,       // Mua vật phẩm từ thương nhân
    SHOP_SELL       // Bán vật phẩm trong túi cho thương nhân
};

class InventoryShopScene2D {
public:
    InventoryShopScene2D(Hero& hero, Shop* shop = nullptr, InventoryShopMode mode = InventoryShopMode::INVENTORY_ONLY);
    ~InventoryShopScene2D() = default;

    void update(float dt);
    void render();

    bool isClosed() const { return m_closed; }
    void open(InventoryShopMode mode = InventoryShopMode::INVENTORY_ONLY);
    void close() { m_closed = true; }

    void setShop(Shop* shop) { m_shop = shop; }
    void setMode(InventoryShopMode mode) { m_mode = mode; m_selectedIndex = 0; }

private:
    void renderHeroStatsPanel();
    void renderInventoryGrid();
    void renderShopGrid();
    void renderItemDetailPanel();
    void renderNotification();

    void handleInventoryActions();
    void handleShopActions();

    Hero& m_hero;
    Shop* m_shop;
    InventoryShopMode m_mode;
    bool m_closed;

    int m_selectedIndex;
    std::string m_notificationText;
    float m_notificationTimer;
    Color m_notificationColor;

    float m_animTimer;
};

#endif // INVENTORY_SHOP_SCENE_2D_H
