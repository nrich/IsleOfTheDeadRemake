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

#ifndef MAP_H
#define MAP_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>

#include <raylib-cpp.hpp>

#include "Segment.h"

class World;

class Map {
    const std::string filename;
    std::vector<Segment> segments;

    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
public:
    Map(const std::string &filename);

    const std::vector<Segment> &getSegments() const {
        return segments;
    }

    uint16_t getX() const {
        return x;
    }

    uint16_t getY() const {
        return y;
    }

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

    const std::string &getFilename() const {
        return filename;
    }

    void sortSegments(const raylib::Camera3D *camera, World *world);

    ~Map();
};

#endif //MAP_H
