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

#ifndef PANEL_H
#define PANEL_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <array>
#include <unordered_map>

#include <raylib-cpp.hpp>

#include "Player.h"
#include "Fnt.h"

class Panel {
    struct Button {
        raylib::Vector2 position;
        raylib::TextureUnmanaged icon;

        raylib::Rectangle Border() const {
            return raylib::Rectangle(position.GetX(), position.GetY(), icon.GetWidth(), icon.GetHeight());
        }

        void draw(int scale) const {
            icon.Draw(position * scale, 0.0f, scale);
        }
    };

    std::unordered_map<Action, Button> buttons;
    std::unordered_map<Input, Button> movements;

    raylib::TextureUnmanaged background;

    State previous;
public: 
    Panel();
    void draw(Player *player, const std::pair<std::string, std::optional<raylib::Color>> &higlight, int scale);
    ~Panel();
};

#endif //PANEL_H
