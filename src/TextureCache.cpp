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

#include <unordered_map>
#include <iostream>

#include "CelThree.h"
#include "StillCel.h"
#include "TextureCache.h"

static std::unordered_map<std::string, raylib::TextureUnmanaged> cache;

const raylib::TextureUnmanaged &TextureCache::LoadCelThree(const std::string &filename) {
    static Palette palette("cels3/palette.pal");

    if (cache.contains(filename)) {
        return cache[filename];
    }

    raylib::TextureUnmanaged texture = CelThree(filename, palette).getTexture();

    cache.emplace(filename, texture);

    return cache[filename];
}

const raylib::TextureUnmanaged &TextureCache::LoadStillCel(const std::string &filename) {
    if (cache.contains(filename)) {
        return cache[filename];
    }

    raylib::TextureUnmanaged texture = StillCel(filename).getTexture();
    cache.emplace(filename, texture);

    return cache[filename];
}

const std::vector<raylib::TextureUnmanaged> TextureCache::LoadCelThree(const std::vector<std::string> &filenames) {
    std::vector<raylib::TextureUnmanaged> textures;

    for (const auto &filename : filenames) {
        textures.push_back(TextureCache::LoadCelThree(filename));
    }

    return textures;
}

const std::vector<raylib::TextureUnmanaged> TextureCache::LoadStillCel(const std::vector<std::string> &filenames) {
    std::vector<raylib::TextureUnmanaged> textures;

    for (const auto &filename : filenames) {
        textures.push_back(TextureCache::LoadStillCel(filename));
    }

    return textures;
}
