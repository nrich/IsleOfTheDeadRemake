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

#ifndef PALETTE_H
#define PALETTE_H

#include <cstdint>
#include <array>
#include <exception>
#include <string>

#include <raylib-cpp.hpp>

class Palette {
    std::array<raylib::Color, 256> colours;
public:
    Palette(const std::string &filename);
    Palette(const std::array<uint8_t, 768> &data);
    Palette(std::ifstream &fh);

    const raylib::Color operator[](const int index) const {
        if (index < 0 || index > 255)
            throw std::out_of_range(std::string("Index outside 0 <= ") + std::to_string(index) + " <= 255");
        return colours[index];
    }

    ~Palette();
};

#endif //PALETTE_H
