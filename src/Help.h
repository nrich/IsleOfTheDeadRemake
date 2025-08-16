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

#ifndef HELP_H
#define HELP_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

#include <raylib-cpp.hpp>

#include "Player.h"
#include "Fnt.h"
#include "Panel.h"

class Help {
    Panel *panel;
public: 
    Help(Panel *panel);
    void draw(Player *player, raylib::Window &window, int scale);
    ~Help();
};

#endif //HELP_H
