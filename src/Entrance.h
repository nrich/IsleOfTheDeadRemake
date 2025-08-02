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

#ifndef ENTRANCE_H
#define ENTRANCE_H

#include <cstdint>
#include <unordered_map>

#include <raylib-cpp.hpp>

enum Direction : uint32_t {
    NORTH = 0,
    WEST = 90,
    SOUTH = 180,
    EAST = 270,
};

class Entrance {
    std::string name;

    uint32_t x;
    uint32_t y;
    Direction direction;

public:
    Entrance(const std::string &map_name, uint32_t x, uint32_t y, uint32_t direction) : name(map_name), x(x), y(y), direction((Direction)direction) {
    }

    inline const std::string getName() const {
        return name;
    }

    inline const uint32_t X() const {
        return x;
    }

    inline const uint32_t Y() const {
        return y;
    }

    inline const Direction getDirection() const {
        return direction;
    }

public:
    static std::vector<Entrance> Parse(const std::string &filename);
};

#endif //ENTRANCE_H
