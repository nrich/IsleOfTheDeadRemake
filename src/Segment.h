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

#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>
#include <utility>

#include <raylib-cpp.hpp>

struct Segment {
    size_t id;
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
    uint16_t texture;
    uint16_t flags;
    uint16_t count;

    Segment(size_t _id, uint16_t _x1, uint16_t _y1, uint16_t _x2, uint16_t _y2, uint16_t _texture, uint16_t _flags, uint16_t _count) : id(_id), x1(_x1), y1(_y1), x2(_x2), y2(_y2), texture(_texture), flags(_flags), count(_count) {
    }

    void render(const raylib::Camera3D &camera, uint64_t frame_count) const;

    std::pair<raylib::Vector2, float> getBounds() const;
};

#endif //SEGMENT_H
