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

#ifndef TONE_H
#define TONE_H

#include <cstdint>
#include <deque>
#include <optional>

#include <raylib-cpp.hpp>

#include "Flic.h"

class Animation {
    size_t frame = 0;

    Flic flic;
    std::optional<raylib::Sound> sound;
public:
    Animation(const std::string &flic_filename, const std::string &sound_filename);
    Animation(const std::string &flic_filename);

    bool play(const int scale);

    ~Animation();
};

#endif //TONE_H
