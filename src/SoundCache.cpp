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

#include "Voc.h"
#include "SoundCache.h"

raylib::Sound *SoundCache::Load(const std::string &filename) {
    static std::unordered_map<std::string, raylib::Sound> cache;

    if (cache.contains(filename))
        return &cache[filename];

    cache.emplace(filename, Voc::Load(filename));

    return &cache[filename];
}
