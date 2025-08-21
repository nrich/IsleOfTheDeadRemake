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

#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <cstdint>
#include <vector>

#include <raylib-cpp.hpp>

class TextureCache {
public:
    static const raylib::TextureUnmanaged &LoadCelThree(const std::string &filename);
    static const raylib::TextureUnmanaged &LoadStillCel(const std::string &filename);
    static const std::vector<raylib::TextureUnmanaged> LoadCelThree(const std::vector<std::string> &filenames);
    static const std::vector<raylib::TextureUnmanaged> LoadStillCel(const std::vector<std::string> &filenames);
};

#endif //TEXTURECACHE_H
