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

#include <iostream>

#include "Player.h"
#include "Monster.h"
#include "Voc.h"

using namespace Monster;

static void draw_entity(const raylib::Camera3D *camera, const raylib::Vector2 &position, const raylib::TextureUnmanaged &texture) {
    const float y_offset = 5.0f;
    const float scale = 10.0f;

    camera->DrawBillboard(texture, Vector3(position.GetX(), y_offset, position.GetY()), scale);
}

Base::Base(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures, const float step_size) : Entity(segment), textures(textures), stepSize(step_size) {
    int x;
    int y;

    if (x1 == x2) {
        float min_y = std::min(y1, y2);
        float max_y = std::max(y1, y2);

        radius = (max_y - min_y) / 2.0f;

        x = x1;
        y = (max_y + min_y) / 2;
    } else {
        float min_x = std::min(x1, x2);
        float max_x = std::max(x1, x2);

        radius = (max_x - min_x) / 2.0f;

        x = (max_x - min_x) / 2;
        y = y1;
    }

    position.SetX(x);
    position.SetY(y);
}

Collision Base::collide() const {
    if (state == MonsterState::Asleep || state == MonsterState::Dying || state == MonsterState::Dead) {
        return Collision::Pass;
    }

    return Collision::Block;
}

std::optional<std::pair<raylib::Vector2, float>> Base::getBounds() const {
    return std::make_pair(position, radius);
}

std::optional<raylib::Vector2> Base::getPosition() const {
    return position;
}

void Base::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    draw_entity(camera, position, textures[currentFrame]);
}

void Base::update(Player *player, uint64_t frame_count) {
    if (frame_count % 5 == 0)
        currentFrame += 1;

    auto player_position = player->getPosition();
    float distance = position.Distance(player_position);

    if (state == MonsterState::Asleep) {
        if (distance < 50.0f) {
            if (sounds[MonsterSound::Wake])
                sounds[MonsterSound::Wake]->Play();
            state = MonsterState::Waking;
            currentFrame = std::get<0>(stateFrames[state]);
        }
    }

    if (state == MonsterState::Standing || state == MonsterState::Walking) {
        if (distance < 20.0f) {
            state = MonsterState::Attacking;
            currentFrame = std::get<0>(stateFrames[state]);
            if (sounds[MonsterSound::Attack])
                sounds[MonsterSound::Attack]->Play();

            player->takeDamage(5, DeathType::Zombie);
        }
    }

    if (state == MonsterState::Standing) {
        if (distance < 200.0f) {
            state = MonsterState::Walking;
            currentFrame = std::get<0>(stateFrames[state]);
        }
    }

    if (state == MonsterState::Walking) {
        if (distance >= 200.0f) {
            state = MonsterState::Standing;
            currentFrame = std::get<0>(stateFrames[state]);
        } else {
            if (frame_count % 5 == 0) {
                World *world = player->getWorld();
                Level *level = world->getCurrentLevel();

                std::vector<raylib::Vector2> path = level->findPath(position, player_position);

                if (path.size() > 1) {
                    auto next_target = path[1];

                    position = position.MoveTowards(next_target, 75.0 * GetFrameTime());
                } else if (path.size() == 1) {
                    std::cerr << "ONE STEP\n";
                    state = MonsterState::Standing;
                    currentFrame = std::get<0>(stateFrames[state]);
                } else {
                    std::cerr << "NO PATH\n";
                    state = MonsterState::Standing;
                    currentFrame = std::get<0>(stateFrames[state]);
                }
            }
        }
    }

    auto stateFrame = stateFrames[state];

    if (currentFrame > std::get<1>(stateFrame)) {
        auto next_state = std::get<2>(stateFrame);

        if (next_state == MonsterState::Repeat) {
            currentFrame = std::get<0>(stateFrame);
        } else {
            currentFrame = std::get<0>(stateFrames[next_state]);
            state = next_state;
        }
    }
}

void Base::damage(const DamageType damage_type, int amount) {
    if (state == MonsterState::Standing || state == MonsterState::Walking || state == MonsterState::Attacking) {
        health -= amount;

        if (health > 0) {
            state = MonsterState::Hurt;
            currentFrame = std::get<0>(stateFrames[state]);
            if (sounds[MonsterSound::Hurt])
                sounds[MonsterSound::Hurt]->Play();
        } else {
            state = MonsterState::Dying;
            currentFrame = std::get<0>(stateFrames[state]);
            if (sounds[MonsterSound::Death])
                sounds[MonsterSound::Death]->Play();
        }
    }
}

std::optional<raylib::RayCollision> Base::collide(const raylib::Ray &ray) {
    if (collide() == Collision::Pass)
        return std::nullopt;

    const float height = 6.0f; 

    auto collision = ray.GetCollision(raylib::Vector3(position.GetX(), height, position.GetY()), height);
    if (collision.GetDistance())
        return collision;

    return std::nullopt;
}

Base::~Base() {

}

Bat::Bat(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/batwake.voc");
    static raylib::Sound wake_sound(wake_wav);

    health = 5;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 9, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(10, 17, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(10, 17, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(18, 28, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(29, 29, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(29, 36, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(37, 37, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Bat::~Bat() {

}

CJ::CJ(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/fgrowl.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/tap.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/cjdie.voc");
    static raylib::Sound death_sound(death_wav);

    health = 30;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 12, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(13, 13, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(14, 21, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(22, 29, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(30, 33, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(34, 46, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(47, 47, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

CJ::~CJ() {

}

Doc::Doc(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    health = 30;
    state = MonsterState::Standing;

    stateFrames[MonsterState::Standing] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(1, 8, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(9, 19, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(20, 27, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(28, 39, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(40, 40, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Doc::~Doc() {

}

Dude::Dude(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/wake.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/dude.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/dudekill.voc");
    static raylib::Sound death_sound(death_wav);

    health = 30;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 14, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(15, 15, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(16, 23, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(24, 30, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(31, 33, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(34, 38, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(39, 39, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Dude::~Dude() {

}

Harry::Harry(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/wake.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/errhaa.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/harrydie.voc");
    static raylib::Sound death_sound(death_wav);

    health = 30;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 8, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(9, 9, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(10, 17, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(18, 33, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(34, 39, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(40, 45, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(46, 46, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Harry::~Harry() {

}

Kid::Kid(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/daddy.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/errhaa.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/kidkill.voc");
    static raylib::Sound death_sound(death_wav);

    health = 5;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 1, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(1, 1, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(2, 9, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(10, 23, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(24, 24, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(25, 32, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(33, 33, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Kid::~Kid() {

}


Nurse::Nurse(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    health = 30;
    state = MonsterState::Standing;

    stateFrames[MonsterState::Standing] = std::make_tuple(9, 9, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(0, 8, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(10, 16, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(17, 24, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(25, 31, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(32, 32, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Nurse::~Nurse() {

}


Roy::Roy(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/wake.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/errhaa.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/roykill.voc");
    static raylib::Sound death_sound(death_wav);

    health = 30;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 9, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(10, 10, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(11, 17, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(18, 27, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(28, 31, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(32, 42, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(43, 43, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Roy::~Roy() {

}

Tor::Tor(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/wake.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/chomp.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/errhaa.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/tordie.voc");
    static raylib::Sound death_sound(death_wav);

    health = 30;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 15, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(16, 16, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(17, 25, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(26, 37, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(38, 43, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(44, 51, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(52, 52, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Tor::~Tor() {

}

Wolf::Wolf(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    static const raylib::Wave wake_wav = Voc::Load("sound/wolfw.voc");
    static raylib::Sound wake_sound(wake_wav);

    static const raylib::Wave attack_wav = Voc::Load("sound/wolfbite.voc");
    static raylib::Sound attack_sound(attack_wav);

    static const raylib::Wave hurt_wav = Voc::Load("sound/dogdie.voc");
    static raylib::Sound hurt_sound(hurt_wav);

    static const raylib::Wave death_wav = Voc::Load("sound/wolfdie.voc");
    static raylib::Sound death_sound(death_wav);

    health = 5;
    state = MonsterState::Asleep;

    sounds[MonsterSound::Wake] = &wake_sound;
    sounds[MonsterSound::Attack] = &attack_sound;
    sounds[MonsterSound::Hurt] = &hurt_sound;
    sounds[MonsterSound::Death] = &death_sound;

    stateFrames[MonsterState::Asleep] = std::make_tuple(0, 0, MonsterState::Repeat);
    stateFrames[MonsterState::Waking] = std::make_tuple(1, 11, MonsterState::Standing);
    stateFrames[MonsterState::Standing] = std::make_tuple(12, 12, MonsterState::Repeat);
    stateFrames[MonsterState::Walking] = std::make_tuple(13, 20, MonsterState::Repeat);
    stateFrames[MonsterState::Attacking] = std::make_tuple(21, 35, MonsterState::Standing);
    stateFrames[MonsterState::Hurt] = std::make_tuple(36, 36, MonsterState::Standing);
    stateFrames[MonsterState::Dying] = std::make_tuple(36, 43, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(44, 44, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

Wolf::~Wolf() {

}

Drummer::Drummer(const Segment *segment, const std::vector<raylib::TextureUnmanaged> &textures) : Base(segment, textures, 75.0f) {
    health = 1;
    state = MonsterState::Standing;

    stateFrames[MonsterState::Standing] = std::make_tuple(0, 1, MonsterState::Repeat);
    stateFrames[MonsterState::Dying] = std::make_tuple(2, 5, MonsterState::Dead);
    stateFrames[MonsterState::Dead] = std::make_tuple(6, 6, MonsterState::Repeat);

    currentFrame = std::get<0>(stateFrames[state]);
}

void Drummer::update(Player *player, uint64_t frame_count) {
    if (frame_count % 5 == 0)
        currentFrame += 1;

    auto stateFrame = stateFrames[state];

    if (currentFrame > std::get<1>(stateFrame)) {
        auto next_state = std::get<2>(stateFrame);

        if (next_state == MonsterState::Repeat) {
            currentFrame = std::get<0>(stateFrame);
        } else {
            currentFrame = std::get<0>(stateFrames[next_state]);
            state = next_state;
        }
    }
}

Drummer::~Drummer() {

}
