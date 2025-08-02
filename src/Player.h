#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include <iostream>

#include <raylib-cpp.hpp>

#include "Map.h"
#include "World.h"

enum class Action {
    Look,
    Get,
    Talk,
    Use,
    Inventory,
    Help,
};

enum Input : uint64_t {
    PrimaryAction   = 1 << 0,
    ViewMap         = 1 << 1,
    StepForward     = 1 << 2,
    StepBack        = 1 << 3,
    StepLeft        = 1 << 4,
    StepRight       = 1 << 5,
    TurnLeft        = 1 << 6,
    TurnRight       = 1 << 7,
    LookUp          = 1 << 8,
    LookDown        = 1 << 9,
    ViewInventory   = 1 << 10,
    EquipMachete    = 1 << 11,
    EquipRifle      = 1 << 12,
    EquipShotgun    = 1 << 13,
    EquipUzi        = 1 << 14,
};

class Player {
    struct Weapon {
        int attackFrameCount;
        std::vector<raylib::TextureUnmanaged> frames;
        std::function<void(Player *player)> use;
    };

    World *world;

    raylib::Vector2 angles;

    raylib::Camera camera;

    State state;

    std::unordered_map<Item, int> items;

    int health = 75;

    std::optional<Item> selected = std::nullopt;
    std::optional<Action> action = std::nullopt;
    std::optional<Item> weapon = std::nullopt;

    std::unordered_map<Item, Weapon> weapons;

    uint64_t weaponStartFrame = 0;
    uint64_t weaponEndFrame = 0;

    uint64_t input = 0;

    State inventoryReturnState;

    std::string highlight = "";

    std::pair<raylib::Vector3, raylib::Vector3> processInput(const uint64_t frame_count);
    void tryMove(const raylib::Vector3 &movement, const raylib::Vector3 &rotation);

    DeathType deathType;
public:
    Player(World *world);

    raylib::Vector2 getPosition() const {
        auto camera_position = camera.GetPosition();
        return raylib::Vector2(camera_position.x, camera_position.z);
    }

    void setPosition(const raylib::Vector2 &new_position) {
        camera.SetPosition(raylib::Vector3(new_position.GetX(), 6.0f, new_position.GetY()));
        camera.SetTarget(raylib::Vector3(new_position.GetX(), 6.0f, new_position.GetY() - 10));
        angles = raylib::Vector2();
    }

    raylib::Vector2 getAngles() const {
        return angles;
    }

    void setAngles(const raylib::Vector2 &new_angles) {
        camera.Update(raylib::Vector3(), raylib::Vector3(-angles.GetX(), 0.0f, 0.0f));
        camera.Update(raylib::Vector3(), raylib::Vector3(new_angles.GetX(), 0.0f, 0.0f));

        angles = new_angles;
    }

    World *getWorld() {
        return world;
    }

    bool takeStep(const Map &map);

    raylib::Camera *getCamera() {
        return &camera;
    }

    State getState() const {
        return state;
    }

    State getInventoryReturnState() const {
        return inventoryReturnState;
    }

    void setState(State new_state) {
        if (new_state == State::Inventory) {
            inventoryReturnState = state;
            action = Action::Inventory;
        }

        state = new_state;
        input = 0;
    }

    void addItem(Item item, int count=-1);

    int getItemCount(const Item item) const;

    int getHealth() const {
        return health;
    }

    void useItem(const Item item);

    std::optional<Item> getSelectedItem() const {
        return selected;
    }

    void setSelectedItem(std::optional<Item> new_selected) {
        selected = new_selected;
    }

    std::optional<Action> getAction() const {
        return action;
    }

    void setAction(std::optional<Action> new_Action) {
        action = new_Action;
    }

    std::pair<bool, std::string> useItemOnItem(Item source, Item destination);

    void equipWeapon(std::optional<Item> weapon_item);
    std::optional<Item> equippedWeapon() const {
        return weapon;
    }

    void drawWeapon(uint64_t frame_count, int scale) const;
    void useWeapon(uint64_t frame_count);

    void update(const uint64_t frame_count);

    uint64_t getInput() const {
        return input;
    }

    void setInput(uint64_t new_input) {
        input = new_input;
    }

    std::string getHighlight() const {
        return highlight;
    }

    void setHighlight(const std::string &new_highlight="") {
        highlight = new_highlight;
    }

    void takeDamage(const int amount, const DeathType death_type);

    void death(const DeathType death_type) {
        state = State::Dead;
        deathType = death_type;
    }

    DeathType getDeathType() const {
        return deathType;
    }

    void respawn(const raylib::Vector2 &new_position, const bool reset_inventory=false);

    ~Player();
};

#endif //PLAYER_H
