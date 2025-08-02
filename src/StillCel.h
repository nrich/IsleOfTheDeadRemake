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

#ifndef STILLCEL_H
#define STILLCEL_H

#include <cstdint>
#include <array>

#include <raylib-cpp.hpp>

#include "Palette.h"

class StillCel {
    const std::string filename;
    raylib::TextureUnmanaged texture;

    uint16_t width;
    uint16_t height;
public:
    StillCel(const std::string &filename);

    const raylib::TextureUnmanaged &getTexture() const {
        return texture;
    }

    ~StillCel();
};

#endif //STILLCEL_H
