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

#include "Player.h"
#include "Strings.h"
#include "StillCel.h"
#include "Voc.h"

#include <iostream>

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

#ifndef DEG2RAD
#define DEG2RAD (M_PI/180.0)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f/M_PI)
#endif

Player::Player(World *world) : world(world), angles(0, 0), state(State::World), deathType(DeathType::None) {
    camera = raylib::Camera(
        raylib::Vector3(0, 6.0f, 0),
        raylib::Vector3(0, 6.0f, 0),
        raylib::Vector3(0.0f, 1.0f, 0.0f),
        45.0f,
        CAMERA_PERSPECTIVE
    );

    weapons[Item::Rifle] = Weapon(45, {
        StillCel("stillcel/gun1.cel").getTexture(),
        StillCel("stillcel/gun2.cel").getTexture(),
        StillCel("stillcel/gun3.cel").getTexture(),
    }, [](Player *player) {
        player->takeDamage(999, DeathType::Rifle);
    });

    weapons[Item::OiledRifle] = Weapon(55, {
        StillCel("stillcel/gun1.cel").getTexture(),
        StillCel("stillcel/gun2.cel").getTexture(),
        StillCel("stillcel/gun3.cel").getTexture(),
    }, [](Player *player) {
        if (player->items[Item::Ammo1] <= 0)
            return;

        player->items[Item::Ammo1] -= 1;

        static const raylib::Wave wav = Voc::Load("sound/gun.voc");
        static raylib::Sound sound(wav);

        auto *camera = player->getCamera();
        auto ray = camera->GetScreenToWorldRay(raylib::Vector2(160, 100), 320, 200);

        World *world = player->getWorld();
        Level *level = world->getCurrentLevel();
        Map *map = level->getMap();

        sound.Play();

        for (const auto &segment : map->getSegments()) {
            Entity *entity = world->getEntity(segment.id);

            if (entity) {
                auto if_collision = entity->collide(ray);

                if (if_collision) {
                    auto collision = *if_collision;

                    if (collision.GetDistance() < 1000) {
                        entity->damage(player, DamageType::Bullet, 20);
                    }
                }
            }
        }
    }, Item::Ammo1);

    weapons[Item::Shotgun] = Weapon(40, {
        StillCel("stillcel/sgun1.cel").getTexture(),
        StillCel("stillcel/sgun2.cel").getTexture(),
        StillCel("stillcel/sgun3.cel").getTexture(),
    }, [](Player *player) {
        if (player->items[Item::Ammo2] <= 0)
            return;

        player->items[Item::Ammo2] -= 1;

        static const raylib::Wave wav = Voc::Load("sound/shotgun.voc");
        static raylib::Sound sound(wav);

        auto *camera = player->getCamera();
        auto ray = camera->GetScreenToWorldRay(raylib::Vector2(160, 100), 320, 200);

        World *world = player->getWorld();
        Level *level = world->getCurrentLevel();
        Map *map = level->getMap();

        sound.Play();

        for (const auto &segment : map->getSegments()) {
            Entity *entity = world->getEntity(segment.id);

            if (entity) {
                auto if_collision = entity->collide(ray);

                if (if_collision) {
                    auto collision = *if_collision;

                    if (collision.GetDistance() < 1000) {
                        entity->damage(player, DamageType::Bullet, 25);
                    }
                }
            }
        }
    }, Item::Ammo2);

    weapons[Item::Uzi] = Weapon(10, {
        StillCel("stillcel/uzi1.cel").getTexture(),
        StillCel("stillcel/uzi2.cel").getTexture(),
        StillCel("stillcel/uzi3.cel").getTexture(),
    }, [](Player *player) {
        if (player->items[Item::Ammo3] <= 0)
            return;

        player->items[Item::Ammo3] -= 1;

        static const raylib::Wave wav = Voc::Load("sound/uzi.voc");
        static raylib::Sound sound(wav);

        auto *camera = player->getCamera();
        auto ray = camera->GetScreenToWorldRay(raylib::Vector2(160, 100), 320, 200);

        World *world = player->getWorld();
        Level *level = world->getCurrentLevel();
        Map *map = level->getMap();

        sound.Play();

        for (const auto &segment : map->getSegments()) {
            Entity *entity = world->getEntity(segment.id);

            if (entity) {
                auto if_collision = entity->collide(ray);

                if (if_collision) {
                    auto collision = *if_collision;

                    if (collision.GetDistance() < 1000) {
                        entity->damage(player, DamageType::Bullet, 15);
                    }
                }
            }
        }
    }, Item::Ammo3);

    weapons[Item::Machete] = Weapon(40, {
        StillCel("stillcel/machete1.cel").getTexture(),
        StillCel("stillcel/machete2.cel").getTexture(),
        StillCel("stillcel/machete3.cel").getTexture(),
    }, [](Player *player) {
        static const raylib::Wave wav = Voc::Load("sound/whack.voc");
        static raylib::Sound sound(wav);

        auto *camera = player->getCamera();
        auto ray = camera->GetScreenToWorldRay(raylib::Vector2(160, 100), 320, 200);

        World *world = player->getWorld();
        Level *level = world->getCurrentLevel();
        Map *map = level->getMap();

        sound.Play();

        for (const auto &segment : map->getSegments()) {
            Entity *entity = world->getEntity(segment.id);

            if (entity) {
                auto if_collision = entity->collide(ray);

                if (if_collision) {
                    auto collision = *if_collision;

                    if (collision.GetDistance() < 15) {
                        entity->damage(player, DamageType::Machete, 10);
                        break;
                    }
                }
            }
        }
    });

    respawn(raylib::Vector2(), true);
}

bool Player::takeStep(const Map &map) {
    return true;
}

void Player::addItem(Item item, int count) {
    if (items.contains(item)) {
        items[item] += count;
    } else {
        items[item] = count;
    }

    if (item == Item::Shotgun) {
        items[Item::Ammo2] += 25;
    }

    if (item == Item::Uzi) {
        items[Item::Ammo3] += 50;
    }
}

int Player::getItemCount(const Item item) const {
    if (items.contains(item)) {
        return items.at(item);
    }

    return 0;
}

std::pair<bool, std::string> Player::useItemOnItem(Item source, Item destination) {
    if (source == Item::OilCan && destination == Item::Rags) {
        items[Item::Rags] = 0;
        items[Item::OilyRags] = -1;

        return std::make_pair(true, Strings::Lookup(193));
    }

    if (source == Item::OilyRags && destination == Item::Rifle) {
        items[Item::Rifle] = 0;
        items[Item::OiledRifle] = -1;

        return std::make_pair(true, Strings::Lookup(194));
    }

    if (source == Item::Flower && destination == Item::Beaker) {
        items[Item::Beaker] = 0;
        items[Item::Flower] = 0;
        items[Item::BeakerFlower] = -1;

        return std::make_pair(true, Strings::Lookup(186));
    }

    if (source == Item::Chemicals && destination == Item::BeakerFlower) {
        items[Item::BeakerFlower] = 0;
        items[Item::Chemicals] = 0;
        items[Item::BeakerFlowerChemicals] = -1;

        return std::make_pair(true, Strings::Lookup(187));
    }

    if (source == Item::Crystal && destination == Item::BeakerFlowerChemicals) {
        items[Item::BeakerFlowerChemicals] = 0;
        items[Item::Crystal] = 0;
        items[Item::BeakerFlowerChemicalsCrystal] = -1;

        return std::make_pair(true, Strings::Lookup(188));
    }

    if (source == Item::Syringe && destination == Item::BeakerFlowerChemicalsCrystal) {
        items[Item::Syringe] = 0;
        items[Item::Antidote] = -1;

        return std::make_pair(true, Strings::Lookup(189));
    }

    if (source == Item::Drug && destination == Item::Companion) {
        this->setFlag(Flag::CompanionCalmed);
        items[Item::Drug] = 0;

        return std::make_pair(true, Strings::Lookup(192));
    }

    if (source == Item::Antidote && destination == Item::Companion) {
        this->setFlag(Flag::CompanionCured);
        items[Item::Antidote] = 0;

        return std::make_pair(true, Strings::Lookup(190));
    }

    return std::make_pair(false, Strings::Lookup(375));
}

void Player::equipWeapon(std::optional<Item> weapon_item) {
    if (weapon_item) {
        if (getItemCount(*weapon_item)) {
            weapon = *weapon_item;
        }
    } else {
        weapon = std::nullopt;
    }
}

void Player::drawWeapon(uint64_t frame_count, int scale) const {
    raylib::Vector2 position(160-(82/2), 200 - 138);

    if (weapon) {
        if (!weapons.contains(*weapon))
            throw std::invalid_argument("Unknown weapon"); 

        auto weapon_object = weapons.at(*weapon);

        if (weaponEndFrame >= frame_count) {
            uint64_t weapon_time_left = weaponEndFrame - frame_count;
            size_t weapon_frame = ((weapon_object.attackFrameCount - weapon_time_left) / 5) + 1;

            if (weapon_frame >= weapon_object.frames.size())
                weapon_frame = 0;

            weapon_object.frames[weapon_frame].Draw(position * scale, 0.0f, scale);
        } else {
            weapon_object.frames[0].Draw(position * scale, 0.0f, scale);
        }
    }
}

void Player::useWeapon(uint64_t frame_count) {
    if (weapon) {
        if (!weapons.contains(*weapon))
            throw std::invalid_argument("Unknown weapon");

        if (weaponEndFrame < frame_count) {
            auto weapon_object = weapons.at(*weapon);
            auto ammo = weapon_object.ammo;

            if (ammo) {
                if (getItemCount(*ammo) <= 0) {
                    weaponEndFrame = 0;
                    return;
                }
            }

            weapon_object.use(this);
            weaponStartFrame = frame_count;
            weaponEndFrame = frame_count + weapon_object.attackFrameCount;
        }
    } else {
        weaponEndFrame = 0;
    }
}


void Player::use(uint64_t frame_count) {
    Level *level = world->getCurrentLevel();
    Map *map = level->getMap();

    auto ray = camera.GetScreenToWorldRay(raylib::Vector2(160, 100), 320, 200);

    for (const auto &segment : map->getSegments()) {
        Entity *entity = world->getEntity(segment.id);

        if (entity) {
            auto if_collision = entity->collide(ray);

            if (if_collision) {
                auto collision = *if_collision;

                if (collision.GetDistance() < 20) {
                    entity->use(this, selected);
                    break;
                }
            }
        }
    }
}

void Player::useItem(const Item item) {
    switch (item) {
        case Item::Coconut:
            if (getItemCount(item) > 0) {
                health += 50;
                items[item] -= 1;
                if (health > 100)
                    health = 100;
            }
            break;
        case Item::Banana:
            if (getItemCount(item) > 0) {
                health += 30;
                items[item] -= 1;
                if (health > 100)
                    health = 100;
            }
            break;
        case Item::FirstAid:
            if (getItemCount(item) > 0) {
                health += 60;
                items[item] -= 1;
                if (health > 100)
                    health = 100;
            }
            break;
        case Item::GoldMedal1:
        case Item::GoldMedal2:
            items[item] = 0;
            break;
        case Item::Book:
            switch(state) {
                case State::VillageGate1:
                    this->setFlag(Flag::TranslateVillage1);
                    break;
                case State::VillageGate2:
                    this->setFlag(Flag::TranslateVillage2);
                    break;
                case State::Shaman:
                    this->setFlag(Flag::TranslateShaman);
                    break;
                case State::Chief:
                    this->setFlag(Flag::TranslateChief);
                    break;
                default:
                    break;
            }
            break;
        case Item::Raft:
            if (world->getCurrentLevel()->getMap()->getFilename() == "maps/04.map") {
                if (this->testFlag(Flag::CompanionCured)) {
                    world->setCurrentLevel("maps/30.map");
                    auto spawn_point = world->findSpawn();
                    this->setPosition(spawn_point);
                } else {
                    this->takeDamage(999, DeathType::Companion);
                }
            }
            break;
        case Item::FlareGun:
            if (world->getCurrentLevel()->getMap()->getFilename() == "maps/30.map") {
                if (this->getItemCount(Item::Companion)) {
                    this->setState(State::Ending1);
                } else {
                    this->setState(State::Ending);
                }
            }
            break;
        default:
            break;
    }
}

std::pair<raylib::Vector3, raylib::Vector3> Player::processInput(const uint64_t frame_count) {
    uint64_t player_input = this->getInput();

    const float offset_this_frame = 60.0f * GetFrameTime();
    const float rotate_this_frame = 120.0f * GetFrameTime();

    raylib::Vector3 movement(0, 0, 0);
    raylib::Vector3 rotation(0, 0, 0);

    if (player_input & Input::StepForward) {
        movement.SetX(offset_this_frame);
    } else if (player_input & Input::StepBack) {
        movement.SetX(-offset_this_frame);
    }

    if (player_input & Input::StepLeft) {
        movement.SetY(-offset_this_frame);
    } else if (player_input & Input::StepRight) {
        movement.SetY(offset_this_frame);
    }

    if (player_input & Input::TurnLeft) {
        rotation.SetX(-rotate_this_frame);
    } else if (player_input & Input::TurnRight) {
        rotation.SetX(rotate_this_frame);
    }

    if (player_input & Input::LookDown) {
        rotation.SetY(rotate_this_frame);
    } else if (player_input & Input::LookUp) {
        rotation.SetY(-rotate_this_frame);
    }

    if (player_input & Input::ViewInventory) {
        player_input &= ~Input::ViewInventory;
        showInventory(true);
    }

    if (player_input & Input::ViewMap) {
        player_input &= ~Input::ViewMap;
        this->setState(State::Map);
    }

    if (player_input & Input::PrimaryAction) {
        this->useWeapon(frame_count);
    }

    if (player_input & Input::Use) {
        this->use(frame_count);
    }

    if (player_input & Input::EquipMachete) {
        player_input &= ~Input::EquipMachete;
        this->equipWeapon(Item::Machete);
    }

    if (player_input & Input::EquipRifle) {
        player_input &= ~Input::EquipRifle;

        if (this->getItemCount(Item::OiledRifle)) {
            this->equipWeapon(Item::OiledRifle);
        } else {
            this->equipWeapon(Item::Rifle);
        }
    }

    if (player_input & Input::EquipShotgun) {
        player_input &= ~Input::EquipShotgun;
        this->equipWeapon(Item::Shotgun);
    }

    if (player_input & Input::EquipUzi) {
        player_input &= ~Input::EquipUzi;
        this->equipWeapon(Item::Uzi);
    }

    this->setInput(player_input);

    return std::make_pair(movement, rotation);
}

void Player::tryMove(const raylib::Vector3 &movement, const raylib::Vector3 &rotation) {
    auto *world = getWorld();
    auto *level = world->getCurrentLevel();
    auto *map = level->getMap();

    auto camera_position = camera.GetPosition();

    if (rotation.Length()) {
        angles += raylib::Vector2(rotation.GetX(), 0.0f);

        if (angles.GetX() < 0) {
            angles.x += 360.0f;
        }

        if (angles.GetX() >= 360.0f) {
            angles.x -= 360.0f;
        }
 
        camera.Update(raylib::Vector3(), raylib::Vector3(rotation.GetX(), 0.0f, 0.0f), 0.0);
    }

    if (movement.Length()) {
        auto position = raylib::Vector2(camera_position.x, camera_position.z);

        raylib::Vector2 new_move(movement.y, -movement.x);
        new_move = new_move.Rotate(DEG2RAD * angles.GetX());

        auto new_position = position + new_move;
        auto new_movement = movement;

        bool reset = false;

        for (const auto &segment : map->getSegments()) {
            Entity *entity = world->getEntity(segment.id);
            if (entity) {
                auto bounds_if = entity->getBounds();
                if (bounds_if) {
                    auto bounds = *bounds_if;

                    float distance = bounds.first.Distance(new_position);
                    if (bounds.second + radius > distance) {
                        auto collision = entity->collide();

                        if (collision == Collision::Block) {
                            reset = true;
                        } else if (collision == Collision::Touch) {
                            entity->touch(this);
                            reset = true;
                        }
                    }

                    continue;
                }

                // horizontal segments
                if (segment.y1 == segment.y2) {
                    uint16_t x1 = segment.x1;
                    uint16_t x2 = segment.x2;
                    uint16_t y1 = segment.y1;

                    if (x1 > x2) {
                        std::swap(x1, x2);
                    }

                    if (new_position.x >= x1 && new_position.x <= x2) {
                        int north = new_position.y - radius;
                        int south = new_position.y + radius;

                        if (y1 < new_position.y && y1 > north) {
                            auto collision = entity->collide();
                            if (collision == Collision::Block) {
                                reset = true;
                            } else if (collision == Collision::Touch) {
                                reset = true;
                                entity->touch(this);
                            }

                            break;
                        }

                        if (y1 > new_position.y && y1 < south) {
                            auto collision = entity->collide();
                            if (collision == Collision::Block) {
                                reset = true;
                            } else if (collision == Collision::Touch) {
                                reset = true;
                                entity->touch(this);
                            }
                            break;
                        }
                    }
                }

                // vertical segments
                if (segment.x1 == segment.x2) {
                    uint16_t y1 = segment.y1;
                    uint16_t y2 = segment.y2;
                    uint16_t x1 = segment.x1;

                    if (y1 > y2) {
                        std::swap(y1, y2);
                    }

                    if (new_position.y >= y1 && new_position.y <= y2) {
                        int west = new_position.x - radius;
                        int east = new_position.x + radius;

                        if (x1 < new_position.x && x1 > west) {
                            auto collision = entity->collide();
                            if (collision == Collision::Block) {
                                reset = true;
                            } else if (collision == Collision::Touch) {
                                reset = true;
                                entity->touch(this);
                            }
                            break;
                        }

                        if (x1 > new_position.x && x1 < east) {
                            auto collision = entity->collide();
                            if (collision == Collision::Block) {
                                reset = true;
                            } else if (collision == Collision::Touch) {
                                reset = true;
                                entity->touch(this);
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (reset) {
            new_movement = raylib::Vector3();
        }

        camera.Update(new_movement, raylib::Vector3(), 0.0);
    }
}
 
void Player::update(const uint64_t frame_count) {
    auto [movement, rotation] = processInput(frame_count);

    tryMove(movement, rotation);
}

void Player::takeDamage(const int amount, const DeathType death_type) {
    health -= amount;

    health = std::max(health, 0);

    if (!health) {
        death(death_type);
    }
}

void Player::respawn(const raylib::Vector2 &new_position, const bool reset_inventory) {
    if (reset_inventory) {
        items[Item::Rifle] = 0;
        items[Item::Uzi] = 0;
        items[Item::Shotgun] = 0;
        items[Item::Machete] = 0;
        items[Item::Jacket] = 0;

        items[Item::Ammo1] = 0;
        items[Item::Ammo2] = 0;
        items[Item::Ammo3] = 0;
        items[Item::Flower] = 0;
        items[Item::Crystal] = 0;
        items[Item::Compass] = 0;
        items[Item::FlareGun] = 0;

        items[Item::BoltCutters] = 0;
        items[Item::WireClipper] = 0;
        items[Item::Chemicals] = 0;
        items[Item::Beaker] = 0;
        items[Item::Rags] = 0;
        items[Item::OilCan] = 0;
        items[Item::Lighter] = 0;
        items[Item::Smokes] = 0;

        items[Item::Companion] = 0;
        items[Item::Book] = 0;
        items[Item::Syringe] = 0;
        items[Item::Drug] = 0;
        items[Item::FirstAid] = 0;
        items[Item::Coconut] = 0;
        items[Item::Mango] = 0;
        items[Item::Banana] = 0;

        items[Item::Raft] = 0;
        items[Item::GoldMedal1] = 0;
        items[Item::GoldMedal2] = 0;
        items[Item::DeadWolf] = 0;

        items[Item::OiledRifle] = 0;
        items[Item::OilyRags] = 0;

        items[Item::BeakerFlower] = 0;
        items[Item::BeakerFlowerChemicals] = 0;
        items[Item::BeakerFlowerChemicalsCrystal] = 0;

        items[Item::Antidote] = 0;

        equipWeapon(std::nullopt);
    }

    health = 75;

    setPosition(new_position);
    setAngles(0);
}

Player::~Player() {

}
