/******************************************************************************

Copyright (C) 2025 Neil Richardson (nrich@neiltopia.com)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <optional>

#include <raylib-cpp.hpp>

#include "Game.h"
#include "Segment.h"
#include "Entrance.h"

class Player;

class Entity {
protected:
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
public:
    Entity(const Segment *segment) {
        x1 = segment->x1;
        y1 = segment->y1;
        x2 = segment->x2;
        y2 = segment->y2;
    }

    virtual void draw(const raylib::Camera3D *camera, uint64_t frame_count) const = 0;

    virtual Collision collide() const {
        return Collision::Pass;
    }

    virtual void touch(Player *player) {
    }

    virtual void damage(Player *player, const DamageType damage_type, int amount) {
    }

    virtual std::optional<raylib::RayCollision> collide(const raylib::Ray &ray) {
        return std::nullopt;
    }

    virtual void update(Player *player, uint64_t frame_count) {
    }

    virtual std::optional<std::pair<raylib::Vector2, float>> getBounds() const {
        return std::nullopt;
    }

    virtual std::optional<raylib::Vector2> getPosition() const {
        return std::nullopt;
    }

    virtual SegmentType getType() const {
        return SegmentType::Unknown;
    }

    virtual ~Entity();
};

class Wall : public Entity {
    const raylib::TextureUnmanaged &texture;
public:
    Wall(const Segment *segment, const raylib::TextureUnmanaged &texture) : Entity(segment), texture(texture) {
    }

    Collision collide() const {
        return Collision::Block;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;

    SegmentType getType() const {
        return SegmentType::Wall;
    }
};

class DamageableWall : public Entity {
    const raylib::TextureUnmanaged &texture;
    const raylib::TextureUnmanaged &damaged;
    Collision collision;
    bool isDamaged = false;
public:
    DamageableWall(const Segment *segment, const raylib::TextureUnmanaged &texture, const raylib::TextureUnmanaged &damaged) : Entity(segment), texture(texture), damaged(damaged) {
    }

    Collision collide() const {
        return Collision::Block;
    }

    std::optional<raylib::RayCollision> collide(const raylib::Ray &ray);
    void damage(Player *player, const DamageType damage_type, int amount);
    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;

    SegmentType getType() const {
        return SegmentType::Wall;
    }
};

class AnimatedWall : public Entity {
    const std::vector<raylib::TextureUnmanaged> &textures;
    uint32_t frameRate;

public:
    AnimatedWall(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures, uint32_t frame_rate) : Entity(segment), textures(textures), frameRate(frame_rate) {
    }

    Collision collide() const {
        return Collision::Block;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;

    SegmentType getType() const {
        return SegmentType::Wall;
    }
};

class Portal : public Entity {
    Entrance entrance;
public:
    Portal(const Segment *segment, const Entrance &entrance) : Entity(segment), entrance(entrance) {
    }

    Collision collide() const {
        return Collision::Touch;
    }

    //void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    void touch(Player *player);

    SegmentType getType() const {
        return SegmentType::Door;
    }
};

class Passage : public Portal {
    const raylib::TextureUnmanaged &texture;
public:
    Passage(const Segment *segment, const raylib::TextureUnmanaged &texture, const Entrance &entrance) : Portal(segment, entrance), texture(texture) {
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
};

class Door : public Portal {
protected:
    bool open = false;
    const raylib::TextureUnmanaged &closedTexture;
    const raylib::TextureUnmanaged &openedTexture;
public:
    Door(const Segment *segment, const raylib::TextureUnmanaged &closed_texture, const raylib::TextureUnmanaged &opened_texture, const Entrance &entrance) : Portal(segment, entrance), closedTexture(closed_texture), openedTexture(opened_texture) {
    }

    Collision collide() const {
        if (open)
            return Collision::Touch;

        return Collision::Block;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
};

class Barricade : public Door {
    DamageType expected;
public: 
    Barricade(const Segment *segment, const raylib::TextureUnmanaged &closed_texture, const raylib::TextureUnmanaged &opened_texture, const Entrance &entrance, DamageType expected) : Door(segment, closed_texture, opened_texture, entrance), expected(expected) {
    }

    std::optional<raylib::RayCollision> collide(const raylib::Ray &ray);

    void damage(Player *player, const DamageType damage_type, int amount) {
        if (damage_type == expected) {
            open = true;
        }
    }
};

class RoomEntry : public Wall {
    State scene;
public:
    RoomEntry(const Segment *segment, const raylib::TextureUnmanaged &texture, const State scene) : Wall(segment, texture), scene(scene) {
    }

    Collision collide() const {
        return Collision::Touch;
    }

    void touch(Player *player);

    SegmentType getType() const {
        return SegmentType::Door;
    }
};


class AnimatedRoomEntry : public AnimatedWall {
    State scene;
public:
    AnimatedRoomEntry(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures, uint32_t frame_rate, const State scene) : AnimatedWall(segment, textures, frame_rate), scene(scene) {
    }

    Collision collide() const {
        return Collision::Touch;
    }

    void touch(Player *player);

    SegmentType getType() const {
        return SegmentType::Door;
    }
};

class BarricadedRoomEntry : public Entity {
    bool open = false;

    const raylib::TextureUnmanaged &closedTexture;
    const raylib::TextureUnmanaged &openedTexture;
    DamageType expected;
    State scene;
public:
    BarricadedRoomEntry(const Segment *segment, const raylib::TextureUnmanaged &closed_texture, const raylib::TextureUnmanaged &opened_texture, DamageType expected, State scene) : Entity(segment), closedTexture(closed_texture), openedTexture(opened_texture), expected(expected), scene(scene) {
    }

    void damage(Player *player, const DamageType damage_type, int amount) {
        std::cerr << "HERE\n";

        if (damage_type == expected) {
            open = true;
        }
    }

    Collision collide() const {
        if (open)
            return Collision::Touch;

        return Collision::Block;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    void touch(Player *player);
    std::optional<raylib::RayCollision> collide(const raylib::Ray &ray);

    SegmentType getType() const {
        return SegmentType::Door;
    }
};

class Prop : public Entity {
    const raylib::TextureUnmanaged &texture;
    Collision collision;
public:
    Prop(const Segment *segment, const raylib::TextureUnmanaged &texture, Collision collision) : Entity(segment), texture(texture), collision(collision) {
    }

    Collision collide() const {
        return collision;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;

    SegmentType getType() const {
        return SegmentType::Prop;
    }
};

class DamageableProp : public Entity {
    const raylib::TextureUnmanaged &texture;
    const raylib::TextureUnmanaged &damaged;
    Collision collision;

    bool isDamaged = false;

    raylib::Vector2 position;
    float radius;
public:
    DamageableProp(const Segment *segment, const raylib::TextureUnmanaged &texture, const raylib::TextureUnmanaged &damaged, Collision collision) : Entity(segment), texture(texture), damaged(damaged), collision(collision) {
        int x = 0;
        int y = 0;

        if (x1 == x2) {
            float min_y = std::min(y1, y2);
            float max_y = std::max(y1, y2);

            radius = (max_y - min_y) / 2.0f;

            x = x1;
            y = min_y + (int)radius;
        } else {
            float min_x = std::min(x1, x2);
            float max_x = std::max(x1, x2);

            radius = (max_x - min_x) / 2.0f;

            x = min_x + (int)radius;
            y = y1;
        }

        position.SetX(x);
        position.SetY(y);
    }

    Collision collide() const {
        if (isDamaged)
            return Collision::Pass;

        return collision;
    }

    void damage(Player *player, const DamageType damage_type, int amount);
    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;
    std::optional<raylib::RayCollision> collide(const raylib::Ray &ray);

    SegmentType getType() const {
        return SegmentType::Prop;
    }
};

class AnimatedProp : public Entity {
    const std::vector<raylib::TextureUnmanaged> &textures;
    uint32_t frameRate;
    Collision collision;
public:
    AnimatedProp(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures, uint32_t frame_rate, Collision collision) : Entity(segment), textures(textures), frameRate(frame_rate), collision(collision) {
    }

    Collision collide() const {
        return collision;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;

    SegmentType getType() const {
        return SegmentType::Prop;
    }
};

class Trap : public Entity {
    const raylib::TextureUnmanaged &texture;
    DeathType deathType;
    bool triggered = false;
public:
    Trap(const Segment *segment, const raylib::TextureUnmanaged &texture, const DeathType death_type) : Entity(segment), texture(texture), deathType(death_type) {
    }

    Collision collide() const {
        if (triggered)
            return Collision::Pass;

        return Collision::Touch;
    }

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    void touch(Player *player);
    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;

    SegmentType getType() const {
        return SegmentType::Trap;
    }
};

class ItemPickup : public Entity {
    const raylib::TextureUnmanaged &texture;

    Item item;
    int count;

    bool taken = false;
public:
    ItemPickup(const Segment *segment, const raylib::TextureUnmanaged &texture, const Item item, int count) : Entity(segment), texture(texture), item(item), count(count) {
        x1 = segment->x1;
        y1 = segment->y1;
        x2 = segment->x2;
        y2 = segment->y2;
    }

    Collision collide() const {
        if (taken)
            return Collision::Pass;

        return Collision::Touch;
    }

    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;

    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    void touch(Player *player);

    SegmentType getType() const {
        return SegmentType::Item;
    }
};

#endif //ENTITY_H
