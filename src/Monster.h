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

#ifndef MONSTER_H
#define MONSTER_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <tuple>

#include <raylib-cpp.hpp>

#include "Entity.h"

namespace Monster {

enum class MonsterState {
    Asleep,
    Waking,
    Standing,
    Walking,
    Attacking,
    Hurt,
    Dying,
    Dead,
    Repeat,
};

enum class MonsterSound {
    Wake,
    Attack,
    Hurt,
    Death,
};

class Base : public Entity {
protected:
    uint64_t currentFrame = 0;
    raylib::Vector2 position;
    float radius;
    const std::vector<raylib::TextureUnmanaged> &textures;
    MonsterState state;
    std::unordered_map<MonsterState, std::tuple<size_t, size_t, MonsterState>> stateFrames;
    std::unordered_map<MonsterSound, raylib::Sound*> sounds;
    int32_t health = 0;
    float stepSize;
    DeathType deathType;
    int attackDamage;

    float noticeDistance;
    float attackDistance;
    float walkDistance; 

    virtual void onDeath(Player *player) {
    }
public:
    Base(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures, const float step_size, const DeathType death_type, const int attack_damage, float notice_distance, float attack_distance, float walk_distance);

    void damage(Player *player, const DamageType damage_type, int amount);
    std::optional<raylib::RayCollision> collide(const raylib::Ray &ray);

    std::optional<std::pair<raylib::Vector2, float>> getBounds() const;
    std::optional<raylib::Vector2> getPosition() const;

    Collision collide() const;
    void draw(const raylib::Camera3D *camera, uint64_t frame_count) const;
    virtual void update(Player *player, uint64_t frame_count);

    SegmentType getType() const {
        return SegmentType::Monster;
    }

    ~Base();
};

class Bat : public Base {
public:
    Bat(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class CJ : public Base {
public:
    CJ(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Doc : public Base {
public:
    Doc(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Dude : public Base {
public:
    Dude(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Harry : public Base {
public:
    Harry(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Kid : public Base {
public:
    Kid(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Nurse : public Base {
public:
    Nurse(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Roy : public Base {
public:
    Roy(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Tor : public Base {
public:
    Tor(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Wolf : public Base {
public:
    Wolf(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

class Drummer : public Base {
    void onDeath(Player *player);
public:
    Drummer(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
    void update(Player *player, uint64_t frame_count);
};

class Tank : public Base {
    void onDeath(Player *player);
public:
    Tank(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures);
};

};

#endif //MONSTER_H

