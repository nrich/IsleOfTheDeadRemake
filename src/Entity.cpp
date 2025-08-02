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

#include "Entity.h"
#include "Player.h"

static void draw_wall(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const raylib::TextureUnmanaged &texture) {
    const float height = 12.0f;

    rlBegin(RL_TRIANGLES);
    rlSetTexture(texture.id);

    if (y1 == y2) {
        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(x1, 0, y1);

        rlTexCoord2f(1.0, 1.0);
        rlVertex3f(x2, 0, y1);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(x2, height, y1);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(x2, height, y1);

        rlTexCoord2f(0.0, 0.0);
        rlVertex3f(x1, height, y1);

        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(x1, 0, y1);
    } else {
        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(x1, 0, y1);

        rlTexCoord2f(1.0, 1.0);
        rlVertex3f(x1, 0, y2);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(x1, height, y2);

        rlTexCoord2f(1.0, 0.0);
        rlVertex3f(x1, height, y2);

        rlTexCoord2f(0.0, 0.0);
        rlVertex3f(x1, height, y1);

        rlTexCoord2f(0.0, 1.0);
        rlVertex3f(x1, 0, y1);
    }

    rlEnd();
}

static void draw_entity(const raylib::Camera3D *camera, const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const raylib::TextureUnmanaged &texture) {
    const float y_offset = 5.0f;
    const float scale = 10.0f;

    float mid_x = x1;
    float mid_y = y1;

    camera->DrawBillboard(texture, Vector3(mid_x, y_offset, mid_y), scale);
}

Entity::~Entity() {

}

void Wall::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    draw_wall(x1, y1, x2, y2, texture);
}

void AnimatedWall::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    size_t anim = frame_count / frameRate; 

    auto texture = textures[anim % textures.size()];

    draw_wall(x1, y1, x2, y2, texture);
}

void AnimatedRoomEntry::touch(Player *player) {
    player->setState(scene);
}

void Portal::touch(Player *player) {
    World *world = player->getWorld();
    world->setCurrentLevel(entrance.getName());
    player->setPosition(raylib::Vector2(entrance.X(), entrance.Y()));
    player->setAngles(raylib::Vector2(entrance.getDirection(), 0));
}

void Passage::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    draw_wall(x1, y1, x2, y2, texture);
}

void Door::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    if (open)
        draw_wall(x1, y1, x2, y2, openedTexture);
    else
        draw_wall(x1, y1, x2, y2, closedTexture);
}

std::optional<raylib::RayCollision> Barricade::collide(const raylib::Ray &ray) {
    const float height = 12.0f;

    if (y1 == y2) {
        auto a = raylib::Vector3(x1, 0, y1);
        auto b = raylib::Vector3(x2, 0, y1);
        auto c = raylib::Vector3(x2, height, y1);
        auto d = raylib::Vector3(x1, height, y1);

        auto collision = ray.GetCollision(a, b, c, d);
        if (collision.GetDistance())
            return collision;
    } else {
        auto a = raylib::Vector3(x1, 0, y1);
        auto b = raylib::Vector3(x1, 0, y2);
        auto c = raylib::Vector3(x1, height, y2);
        auto d = raylib::Vector3(x1, height, y1);

        auto collision = ray.GetCollision(a, b, c, d);
        if (collision.GetDistance())
            return collision;
    }

    return std::nullopt;
}


void BarricadedRoomEntry::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    if (open)
        draw_wall(x1, y1, x2, y2, openedTexture);
    else
        draw_wall(x1, y1, x2, y2, closedTexture);
}

void BarricadedRoomEntry::touch(Player *player) {
    player->setState(scene);
}

std::optional<raylib::RayCollision> BarricadedRoomEntry::collide(const raylib::Ray &ray) {
    const float height = 12.0f;

    if (y1 == y2) {
        auto a = raylib::Vector3(x1, 0, y1);
        auto b = raylib::Vector3(x2, 0, y1);
        auto c = raylib::Vector3(x2, height, y1);
        auto d = raylib::Vector3(x1, height, y1);

        auto collision = ray.GetCollision(a, b, c, d);
        if (collision.GetDistance())
            return collision;
    } else {
        auto a = raylib::Vector3(x1, 0, y1);
        auto b = raylib::Vector3(x1, 0, y2);
        auto c = raylib::Vector3(x1, height, y2);
        auto d = raylib::Vector3(x1, height, y1);

        auto collision = ray.GetCollision(a, b, c, d);
        if (collision.GetDistance())
            return collision;
    }

    return std::nullopt;
}

std::optional<std::pair<raylib::Vector2, float>> ItemPickup::getBounds() const {
    if (y1 == y2) {
        float min_x = std::min(x1, x2);
        float max_x = std::max(x1, x2);

        float radius = (max_x - min_x) / 2.0f;
        float centre_x = min_x + radius;

        return std::make_pair(raylib::Vector2(centre_x, y1), radius);
    }

    float min_y = std::min(y1, y2);
    float max_y = std::max(y1, y2);

    float radius = (max_y - min_y) / 2.0f;
    float centre_y = min_y + radius;

    return std::make_pair(raylib::Vector2(x1, centre_y), radius);
}

void ItemPickup::draw(const raylib::Camera3D *camera, uint64_t frame_count) const {
    if (taken)
        return;

    draw_entity(camera, x1, y1, x2, y2, texture); 
}

void ItemPickup::touch(Player *player) {
    player->addItem(item, count);
    taken = true;
}
